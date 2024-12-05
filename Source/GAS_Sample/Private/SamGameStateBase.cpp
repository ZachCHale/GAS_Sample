// Copyright ZH_Dev


#include "SamGameStateBase.h"

#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Character/SamCharacterPlayer.h"
#include "Controller/SamPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/SamPlayerState.h"
#include "UI/HUD/SamHUD.h"

ASamGameStateBase::ASamGameStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	CurrentGameTimerValueSeconds = MatchLengthSeconds;
	QueuedLevelUps = 0;
}

void ASamGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASamGameStateBase, SharedPlayerLevel);
	DOREPLIFETIME(ASamGameStateBase, SharedPlayerExp);
}

void ASamGameStateBase::AddToExp(int32 AddedExp)
{
	if(!HasAuthority()) return;
	SharedPlayerExp += AddedExp;  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}

void ASamGameStateBase::AddToLevel(int32 AddedLevels)
{
	if(!HasAuthority()) return;
	if(AddedLevels <= 0) return;
	StateStatus = EGameStateStatus::LevelUpSelection;
	QueuedLevelUps = AddedLevels;
	QueuedLevelUps--;
	SharedPlayerLevel++;
	Auth_GenerateUpgradesForPlayers();
	//Broadcast Events to clients
	Multicast_StartLevelUpEvent(SharedPlayerLevel, SharedPlayerExp, LevelUpSelectionState);
	//Pause Game
	if(SharedPlayerLevel != 0)
	{
		GetWorld()->GetFirstPlayerController()->SetPause(true);
	}
}

int32 ASamGameStateBase::FindLevelForExp(int32 ExpValue)
{
	check(LevelUpInfo)
	return LevelUpInfo->FindLevelFromTotalExp(ExpValue);
}

EGameStateStatus ASamGameStateBase::GetGameStatus() const
{
	return StateStatus;
}

void ASamGameStateBase::Multicast_GameLost_Implementation()
{
	GameLostDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_GameWon_Implementation()
{
	GameWonDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_StartNewGame_Implementation()
{
	StateStatus = EGameStateStatus::Gameplay;
	if(HasAuthority())
	{
		GetWorld()->GetFirstPlayerController()->SetPause(false);
		for (APlayerState* PS : PlayerArray)
		{
			Auth_ClearPlayerLevelUpSelection(PS);
		}
	}
	NewGameStartDelegate.Broadcast();
}

TArray<ACharacter*> ASamGameStateBase::GetAllPlayerCharacters()
{
	TArray<TObjectPtr<ACharacter>> Characters;

	for (auto PS : PlayerArray)
	{
		if(APlayerController* PC = PS->GetPlayerController())
		{
			if(ACharacter* Character = PC->GetCharacter())
			{
				Characters.AddUnique(Character);
			}
		}
	}
	return Characters;
}

float ASamGameStateBase::GetLastSyncedGameTime() const
{
	return CurrentGameTimerValueSeconds;
}

float ASamGameStateBase::Auth_GetCurrentGameProgress()
{
	if(!HasAuthority()) return 0.f;
	return 1.f-(CurrentGameTimerValueSeconds/MatchLengthSeconds);
}

void ASamGameStateBase::Multicast_EndLevelUpEvent_Implementation(int32 NewLevel)
{
	EndLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_UpdateReadyCount_Implementation(int32 NewReady, int32 NewTotal)
{
	PlayerReadyCount = NewReady;
	PlayerReadyCountChangedDelegate.Broadcast(NewReady, NewTotal);
}

void ASamGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	if (!PlayerState->IsInactive())
	{
		if(!PlayerArray.Contains(PlayerState))
		{
			PlayerArray.AddUnique(PlayerState);
			LevelUpSelectionState.Add(FPlayerLevelUpSelectionState(PlayerState));
			PlayerReadyCountChangedDelegate.Broadcast(PlayerReadyCount, PlayerArray.Num());
			ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
			SamPS->OnPlayerCharacterDeathDelegate.AddUObject(this, &ThisClass::HandlePlayerDeath);
			
		}
	}
}

void ASamGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	for(int i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(PlayerState == LevelUpSelectionState[i].PlayerState)
		{
			LevelUpSelectionState.RemoveAt(i);
			PlayerReadyCountChangedDelegate.Broadcast(PlayerReadyCount, PlayerArray.Num());
			return;
		}
	}
}

void ASamGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	if(GEngine->GetNetMode(GetWorld()) == NM_Standalone)
	{
		StateStatus = EGameStateStatus::Gameplay;
	}
	else
	{
		//Start the game paused in pregame lobby
		StateStatus = EGameStateStatus::PreGameLobby;
		GetWorld()->GetFirstPlayerController()->SetPause(true);
	}
}

void ASamGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Auth_Tick(DeltaSeconds);
}

void ASamGameStateBase::Auth_Tick(float DeltaSeconds)
{
	if(!HasAuthority())return;

	CurrentGameTimerValueSeconds-=DeltaSeconds;
	if(CurrentGameTimerValueSeconds <= 0)
	{
		Auth_EndGameWin();
	}
}

void ASamGameStateBase::Auth_SendPlayerLevelUpSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag)
{
	check(HasAuthority());
	if(StateStatus != EGameStateStatus::LevelUpSelection)return;
	
	if(FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState))
	{
		PlayerSelectionState->bIsReady = true;
		PlayerSelectionState->CurrentlySelectedChoice = UpgradeTag;
	}

	PlayerReadyCount = CountPlayerSelectionsReady();
	Multicast_UpdateReadyCount(PlayerReadyCount, PlayerArray.Num());
	
	if(PlayerReadyCount == PlayerArray.Num())
	{
		Auth_SubmitAllPlayerUpgradeSelections();
	}
}

void ASamGameStateBase::Auth_SetPlayerReadyInLobby(const APlayerState* PlayerState)
{
	check(HasAuthority());
	if(StateStatus != EGameStateStatus::PreGameLobby)return;

	//Repurpose the Selection state since it has a ready up bool
	if(FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState))
	{
		PlayerSelectionState->bIsReady = true;
	}

	PlayerReadyCount = CountPlayerSelectionsReady();
	Multicast_UpdateReadyCount(PlayerReadyCount, PlayerArray.Num());
	if(PlayerReadyCount == PlayerArray.Num())
	{
		Multicast_StartNewGame();
	}

}

bool ASamGameStateBase::IsValidUpgradeSelection(const APlayerState* PlayerState, FGameplayTag UpgradeTag)
{
	FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState);
	if(!PlayerSelectionState || !PlayerSelectionState->UpgradeChoiceTags.Contains(UpgradeTag)) return false;
	return true;
}

void ASamGameStateBase::Auth_ClearPlayerLevelUpSelection(APlayerState* PlayerState)
{
	check(HasAuthority());
	if(FPlayerLevelUpSelectionState* PlayerSelectionState = GetPlayerLevelUpSelectionState(PlayerState))
	{
		PlayerSelectionState->bIsReady = false;
		PlayerSelectionState->CurrentlySelectedChoice = FGameplayTag();
	}
	PlayerReadyCount = CountPlayerSelectionsReady();
	Multicast_UpdateReadyCount(PlayerReadyCount, PlayerArray.Num());
}

TArray<FUpgradeInfoItem> ASamGameStateBase::GetLocalPlayerSelectionChoices()
{
	TArray<FUpgradeInfoItem> ChoicesInfo;
	for (auto PlayerSelectionState : LevelUpSelectionState)
	{
		//If on client, remote player states won't have a controller
		if(PlayerSelectionState.PlayerState->GetPlayerController() == nullptr)
			continue;
		if(PlayerSelectionState.PlayerState->GetPlayerController()->IsLocalPlayerController())
		{
			for (FGameplayTag Tag : PlayerSelectionState.UpgradeChoiceTags)
			{
				ChoicesInfo.Add(UpgradeInfo->GetUpgradeInfoFromTag(Tag));
			}
			break;
		}
	}
	return ChoicesInfo;
}

void ASamGameStateBase::Multicast_StartLevelUpEvent_Implementation(int32 NewLevel, int32 NewExp, const TArray<FPlayerLevelUpSelectionState>& UpdatedLevelUpSelectionState)
{
	//Replication doesn't seem to work when the game is paused, so we need to manually replicate from the RPC
	SharedPlayerExp = NewExp;
	OnRep_SharedPlayerExp();
	SharedPlayerLevel = NewLevel;
	OnRep_SharedPlayerLevel();
	
	//Server generates the upgrade choices and sends them via this RPC
	//Replicate the relevant data for Upgrade selection before broadcasting UpgradeSelectionDelegate.
	LevelUpSelectionState = UpdatedLevelUpSelectionState;
	
	BeginLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_GamePausedByPlayer_Implementation()
{
	GamePausedByPlayerDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_GameUnpausedByPlayer_Implementation()
{
	GameUnpausedByPlayerDelegate.Broadcast();
}

void ASamGameStateBase::OnRep_SharedPlayerLevel() const
{
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);  
}  
  
void ASamGameStateBase::OnRep_SharedPlayerExp() const
{
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}

void ASamGameStateBase::HandlePlayerDeath(ASamPlayerState* PlayerState)
{
	//Check how many players are dead and how many exist
	int32 PlayersInGame = PlayerArray.Num();
	int32 DeadPlayers = 0;
	for (APlayerState* PS : PlayerArray)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PS);
		if(!SamPS->HasLivingCharacter())
		{
			DeadPlayers++;
		}
	}
	if(DeadPlayers >= PlayersInGame)
	{
		Auth_EndGameLoss();
	}
}

void ASamGameStateBase::Auth_EndGameLoss()
{
	if(!HasAuthority())return;
	if(StateStatus == GameEnd) return;
	GetWorld()->GetFirstPlayerController()->SetPause(true);
	StateStatus = GameEnd;
	//Notify Clients To Show End Screens
	Multicast_GameLost();
}

void ASamGameStateBase::Auth_EndGameWin()
{
	if(!HasAuthority())return;
	if(StateStatus == GameEnd) return;
	GetWorld()->GetFirstPlayerController()->SetPause(true);
	StateStatus = GameEnd;
	//Notify Clients To Show End Screens
	Multicast_GameWon();
}

void ASamGameStateBase::Auth_PauseGame()
{
	if(!HasAuthority()) return;
	if(StateStatus != Gameplay) return;
	GetWorld()->GetFirstPlayerController()->SetPause(true);
	StateStatus = PausedByPlayer;
	Multicast_GamePausedByPlayer();
}

void ASamGameStateBase::Auth_UnpauseGame()
{
	if(!HasAuthority()) return;
	if(StateStatus != PausedByPlayer) return;
	GetWorld()->GetFirstPlayerController()->SetPause(false);
	StateStatus = Gameplay;
	Multicast_GameUnpausedByPlayer();
}

void ASamGameStateBase::OnRep_PlayerReadyCount() const
{
	PlayerReadyCountChangedDelegate.Broadcast(PlayerReadyCount, PlayerArray.Num());
}

void ASamGameStateBase::Auth_GenerateUpgradesForPlayers()
{
	if(!HasAuthority()) return;
	for (auto& PlayerLevelUpState : LevelUpSelectionState)
	{
		PlayerLevelUpState.ResetSelectionState();
		PlayerLevelUpState.UpgradeChoiceTags = UpgradeInfo->GetRandomUpgradeTags(3);
	}

}

FPlayerLevelUpSelectionState* ASamGameStateBase::GetPlayerLevelUpSelectionState(const APlayerState* PlayerState)
{
	for (int i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		if(LevelUpSelectionState[i].PlayerState == PlayerState)
		{
			return &LevelUpSelectionState[i];
		}
	}
	return nullptr;
}

int32 ASamGameStateBase::CountPlayerSelectionsReady()
{
	int32 ReadyCount = 0;
	for (auto PlayerLevelUpSelection : LevelUpSelectionState)
	{
		if(PlayerLevelUpSelection.bIsReady) ReadyCount++;
	}
	return ReadyCount;
}

void ASamGameStateBase::Auth_SubmitAllPlayerUpgradeSelections()
{
	if(!HasAuthority()) return;
	//Increase Level / Create new effect for selected upgrades.
	for(int32 i = 0; i < LevelUpSelectionState.Num(); i++)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(LevelUpSelectionState[i].PlayerState);
		USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(SamPS->GetAbilitySystemComponent());
		SamASC->Auth_IncrementUpgradeEffect(LevelUpSelectionState[i].CurrentlySelectedChoice);
		LevelUpSelectionState[i].ResetSelectionState();
	}
	
	//Handle Simultaneous Level ups, or resume gameplay
	if(QueuedLevelUps <=0)
	{
		StateStatus = EGameStateStatus::Gameplay;
		Multicast_EndLevelUpEvent(SharedPlayerLevel);
		GetWorld()->GetFirstPlayerController()->SetPause(false);
	}
	else
	{
		AddToLevel(QueuedLevelUps);
	}
}




