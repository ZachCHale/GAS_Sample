// Copyright ZH_Dev


#include "SamGameStateBase.h"

#include "SamGameplayTags.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelProgressionAsset.h"
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

void ASamGameStateBase::Auth_AddToExp(int32 AddedExp)
{
	if(!HasAuthority()) return;
	SharedPlayerExp += AddedExp;  
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}

void ASamGameStateBase::Auth_AddToLevel(int32 AddedLevels)
{
	if(!HasAuthority()) return;
	if(AddedLevels <= 0) return;
	StateStatus = EGameStateStatus::LevelUpSelection;
	QueuedLevelUps = AddedLevels;
	QueuedLevelUps--;
	SharedPlayerLevel++;
	for (auto PS : PlayerArray)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PS);
		SamPS->Server_StartNewUpgradeState();
	}
	//Auth_GenerateUpgradesForPlayers();
	//Broadcast Events to clients
	Multicast_StartLevelUpEvent(SharedPlayerLevel, SharedPlayerExp);
	Multicast_UpdateUpgradeSelectionReadyCount(0, PlayerArray.Num());
	//Pause Game
	if(SharedPlayerLevel != 0)
	{
		GetWorld()->GetFirstPlayerController()->SetPause(true);
	}
}

int32 ASamGameStateBase::FindLevelForExp(int32 ExpValue)
{
	check(LevelProgressionAsset)
	return LevelProgressionAsset->FindLevelFromTotalExp(ExpValue);
}

EGameStateStatus ASamGameStateBase::GetGameStatus() const
{
	return StateStatus;
}

void ASamGameStateBase::Multicast_GameLost_Implementation()
{
	StateStatus = EGameStateStatus::GameEnd;
	GameLostDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_GameWon_Implementation()
{
	StateStatus = EGameStateStatus::GameEnd;
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
			ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PS);
			SamPS->GetPlayerUpgradeState()->ResetSelectionState();
		}
	}
	Multicast_SyncMatchTime(CurrentGameTimerValueSeconds);
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

float ASamGameStateBase::GetMatchTimeRemaining() const
{
	return CurrentGameTimerValueSeconds;
}

float ASamGameStateBase::GetCurrentGameProgress()
{
	return 1.f-(CurrentGameTimerValueSeconds/MatchLengthSeconds);
}

void ASamGameStateBase::Multicast_EndLevelUpEvent_Implementation(int32 NewLevel)
{
	StateStatus = EGameStateStatus::Gameplay;
	EndLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_UpdateUpgradeSelectionReadyCount_Implementation(int32 NewReady, int32 NewTotal)
{
	PlayerUpgradeReadyCountChangedDelegate.Broadcast(NewReady, NewTotal);
}

void ASamGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	if (!PlayerState->IsInactive())
	{
		if(!PlayerArray.Contains(PlayerState))
		{
			PlayerArray.AddUnique(PlayerState);
			int32 ReadyCount;
			ReadyCount = GetPlayerSelectionsReady();
			Multicast_UpdateUpgradeSelectionReadyCount(ReadyCount, PlayerArray.Num());
			ReadyCount = GetPlayerLobbyReady();
			Multicast_UpdateLobbyReadyCount(ReadyCount, PlayerArray.Num());
			ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
			SamPS->OnPlayerCharacterDeathDelegate.AddUObject(this, &ThisClass::HandlePlayerDeath);
			SamPS->OnAuthUpgradeStateChangedDelegate.AddUObject(this, &ThisClass::HandleAuthPlayerUpgradeStateChanged);
			SamPS->OnAuthLobbyStateChangedDelegate.AddUObject(this, &ThisClass::HandleAuthPlayerLobbyStateChanged);
			
		}
	}
}

void ASamGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	int32 ReadyCount;
	ReadyCount = GetPlayerSelectionsReady();
	PlayerUpgradeReadyCountChangedDelegate.Broadcast(ReadyCount, PlayerArray.Num());
	ReadyCount = GetPlayerLobbyReady();
	PlayerLobbyReadyCountChangedDelegate.Broadcast(ReadyCount, PlayerArray.Num());
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
	if(StateStatus != EGameStateStatus::Gameplay) return;
	CurrentGameTimerValueSeconds-=DeltaSeconds;
	if(HasAuthority() && CurrentGameTimerValueSeconds <= 0)
	{
		Auth_EndGameWin();
	}
}


void ASamGameStateBase:: Multicast_StartLevelUpEvent_Implementation(int32 NewLevel, int32 NewExp)
{
	//Replication doesn't seem to work when the game is paused, so we need to manually replicate from the RPC
	StateStatus = EGameStateStatus::LevelUpSelection;
	SharedPlayerExp = NewExp;
	OnRep_SharedPlayerExp();
	SharedPlayerLevel = NewLevel;
	OnRep_SharedPlayerLevel();
	
	BeginLevelUpSelectionDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_GamePausedByPlayer_Implementation()
{
	StateStatus = EGameStateStatus::PausedByPlayer;
	GamePausedByPlayerDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_GameUnpausedByPlayer_Implementation()
{
	StateStatus = EGameStateStatus::Gameplay;
	GameUnpausedByPlayerDelegate.Broadcast();
}

void ASamGameStateBase::Multicast_UpdateLobbyReadyCount_Implementation(int32 NewReady, int32 NewTotal)
{
	PlayerLobbyReadyCountChangedDelegate.Broadcast(NewReady, NewTotal);
}

void ASamGameStateBase::OnRep_SharedPlayerLevel() const
{
	LevelChangedDelegate.Broadcast(SharedPlayerLevel);  
}  
  
void ASamGameStateBase::OnRep_SharedPlayerExp() const
{
	ExpChangedDelegate.Broadcast(SharedPlayerExp);
}

void ASamGameStateBase::Multicast_SyncMatchTime_Implementation(float TimeRemaining)
{
	CurrentGameTimerValueSeconds = TimeRemaining;
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
	if(StateStatus == EGameStateStatus::GameEnd) return;
	GetWorld()->GetFirstPlayerController()->SetPause(true);
	StateStatus = EGameStateStatus::GameEnd;
	//Notify Clients To Show End Screens
	Multicast_GameLost();
}

void ASamGameStateBase::Auth_EndGameWin()
{
	if(!HasAuthority())return;
	if(StateStatus == EGameStateStatus::GameEnd) return;
	GetWorld()->GetFirstPlayerController()->SetPause(true);
	StateStatus = EGameStateStatus::GameEnd;
	//Notify Clients To Show End Screens
	Multicast_GameWon();
}

void ASamGameStateBase::Auth_PauseGame()
{
	if(!HasAuthority()) return;
	if(StateStatus != EGameStateStatus::Gameplay) return;
	GetWorld()->GetFirstPlayerController()->SetPause(true);
	StateStatus = EGameStateStatus::PausedByPlayer;
	Multicast_GamePausedByPlayer();
}

void ASamGameStateBase::Auth_UnpauseGame()
{
	if(!HasAuthority()) return;
	if(StateStatus != EGameStateStatus::PausedByPlayer) return;
	GetWorld()->GetFirstPlayerController()->SetPause(false);
	StateStatus = EGameStateStatus::Gameplay;
	Multicast_SyncMatchTime(CurrentGameTimerValueSeconds);
	Multicast_GameUnpausedByPlayer();
}

void ASamGameStateBase::HandleAuthPlayerLobbyStateChanged(ASamPlayerState* PlayerState)
{
	check(HasAuthority());
	if(StateStatus != EGameStateStatus::PreGameLobby)return;

	ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerState);
	
	int32 ReadyCount = GetPlayerLobbyReady();
	Multicast_UpdateLobbyReadyCount(ReadyCount, PlayerArray.Num());
	if(ReadyCount == PlayerArray.Num())
	{
		Multicast_StartNewGame();
	}
}

void ASamGameStateBase::HandleAuthPlayerUpgradeStateChanged(ASamPlayerState*)
{
	if(StateStatus != EGameStateStatus::LevelUpSelection)return;

	int32 ReadyCount = GetPlayerSelectionsReady();
	Multicast_UpdateUpgradeSelectionReadyCount(ReadyCount, PlayerArray.Num());
	
	if(ReadyCount == PlayerArray.Num())
	{
		Auth_ApplyAllPlayerUpgradeSelections();
	}
}

int32 ASamGameStateBase::GetPlayerSelectionsReady()
{
	int32 ReadyCount = 0;
	for (auto PS : PlayerArray)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PS);
		if(SamPS->GetPlayerUpgradeState()->bIsReady)
			ReadyCount++;
	}
	return ReadyCount;
}

int32 ASamGameStateBase::GetPlayerLobbyReady()
{
	int32 ReadyCount = 0;
	for (auto PS : PlayerArray)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PS);
		if(SamPS->GetPlayerLobbyState()->bIsReady)
			ReadyCount++;
	}
	return ReadyCount;
}

void ASamGameStateBase::Auth_ApplyAllPlayerUpgradeSelections()
{
	if(!HasAuthority()) return;
	
	//Increase Level / Create new effect for selected upgrades.
	for(int32 i = 0; i < PlayerArray.Num(); i++)
	{
		ASamPlayerState* SamPS = CastChecked<ASamPlayerState>(PlayerArray[i]);
		USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(SamPS->GetAbilitySystemComponent());
		FPlayerUpgradeState* UpgradeState = SamPS->GetPlayerUpgradeState();
		//SamASC->Auth_IncrementUpgradeEffect(UpgradeState->CurrentlySelectedChoice);
		CardDatabase->ExecuteCard(SamPS, UpgradeState->CurrentlySelectedCardTag);
		UpgradeState->ResetSelectionState();
	}
	
	//Handle Simultaneous Level ups, or resume gameplay
	if(QueuedLevelUps <=0)
	{
		StateStatus = EGameStateStatus::Gameplay;
		Multicast_SyncMatchTime(CurrentGameTimerValueSeconds);
		Multicast_EndLevelUpEvent(SharedPlayerLevel);
		GetWorld()->GetFirstPlayerController()->SetPause(false);
	}
	else
	{
		Auth_AddToLevel(QueuedLevelUps);
	}
}




