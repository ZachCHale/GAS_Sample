// Copyright ZH_Dev


#include "Player/SamPlayerState.h"

#include "SamGameplayTags.h"
#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "Character/SamCharacterPlayer.h"
#include "Controller/SamPlayerController.h"
#include "GameFramework/SpectatorPawn.h"

ASamPlayerState::ASamPlayerState()
{
	NetUpdateFrequency = 100.f;
	AbilitySystemComponent = CreateDefaultSubobject<USamAbilitySystemComponent>("AbilitySystemComponent");
	CastChecked<USamAbilitySystemComponent>(AbilitySystemComponent)->SetTeam(ETeam::Player);
	AttributeSet = CreateDefaultSubobject<USamAttributeSet>("AttributeSet");

	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* ASamPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASamPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ASamGameStateBase *SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->AddPlayerState(this);
}

void ASamPlayerState::Destroyed()
{
	Super::Destroyed();
}


UAttributeSet* ASamPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

int32 ASamPlayerState::GetLevel()
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	return SamGS->GetLevel();
}

int32 ASamPlayerState::GetTotalExp()
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	return SamGS->GetTotalExp();
}

void ASamPlayerState::Auth_AddToExp(int32 AddedExp)
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->Auth_AddToExp(AddedExp);
}

  
void ASamPlayerState::Auth_AddToLevel(int32 AddedLevels)  
{  
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->Auth_AddToLevel(AddedLevels);
}

int32 ASamPlayerState::FindLevelForExp(int32 ExpValue)
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	return SamGS->FindLevelForExp(ExpValue);
}

bool ASamPlayerState::HasLivingCharacter() const
{
	AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor();
	if(AvatarActor == nullptr)
		return false;
	ASamCharacterPlayer* PlayerCharacter = CastChecked<ASamCharacterPlayer>(AvatarActor);
	return !PlayerCharacter->GetIsDead();
}

void ASamPlayerState::InitWithPlayerCharacter(ASamCharacterPlayer* PlayerCharacter)
{
	PlayerCharacter->OnDeathDelegate.AddUniqueDynamic(this, &ThisClass::HandleCharacterDeath);
	PlayerCharacter->OnReviveDelegate.AddUniqueDynamic(this, &ThisClass::HandleCharacterRevive);
}

TArray<FGameplayTag> ASamPlayerState::GetAvailableCardTags()
{
	return UpgradeState.UpgradeChoiceTags;
}

FPlayerUpgradeState* ASamPlayerState::GetPlayerUpgradeState()
{
	return &UpgradeState;
}

FPlayerLobbyState* ASamPlayerState::GetPlayerLobbyState()
{
	return &LobbyState;
}

void ASamPlayerState::Auth_ReviveCharacter()
{
	if(!HasAuthority())return;
	ASamCharacterPlayer* PlayerCharacter = CastChecked<ASamCharacterPlayer>(AbilitySystemComponent->GetAvatarActor());
	PlayerCharacter->Auth_Revive();
	Cast<ASamPlayerController>(GetPlayerController())->Auth_StopSpectating();
}

void ASamPlayerState::Auth_SubmitUpgradeSelection(FGameplayTag UpgradeTag)
{
	if(!HasAuthority())return;
	UpgradeState.bIsReady = true;
	UpgradeState.CurrentlySelectedChoice = UpgradeTag;
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	OnAuthUpgradeStateChangedDelegate.Broadcast(this);
}

void ASamPlayerState::Auth_ClearUpgradeSelection()
{
	if(!HasAuthority())return;
	UpgradeState.bIsReady = false;
	UpgradeState.CurrentlySelectedChoice = FGameplayTag();
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	OnAuthUpgradeStateChangedDelegate.Broadcast(this);
}

void ASamPlayerState::Auth_ReadyUpPlayerLobbyState()
{
	LobbyState.bIsReady = true;
	OnAuthLobbyStateChangedDelegate.Broadcast(this);
}

bool ASamPlayerState::IsUpgradeSelectionValid(FGameplayTag UpgradeTag) const
{
	if(!UpgradeState.UpgradeChoiceTags.Contains(UpgradeTag)) return false;
	return true;
}

void ASamPlayerState::Server_StartNewUpgradeState_Implementation()
{
	ASamCharacterPlayer* PlayerCharacter = CastChecked<ASamCharacterPlayer>(AbilitySystemComponent->GetAvatarActor());
	UpgradeState.ResetSelectionState();
	//TODO: Remove Hardcoded number of upgrades to generate. Add ways to increase this number through gameplay.
	//UpgradeState.UpgradeChoiceTags = PlayerCharacter->GetCharacterClassDefaultInfo().UpgradeSelectionInfo->GetRandomUpgradeTags(3);
	if(!HasLivingCharacter())
	{
		//TODO: Make sure there are no bugs related to reviving while the character is in the process of dying (hasn't finished fading out and hasn't switched to spectator)
		UpgradeState.UpgradeChoiceTags.Add(SamTags::ExecCards::ExecCard_Special_Revive);
	}
	else
	{
		// Upgrade Tags
		TArray<FGameplayTag> ValidUpgradeTags;
		// Get all upgrades available for this class
		PlayerCharacter->GetCharacterClassDefaultInfo().AvailableUpgradeCards.GetGameplayTagArray(ValidUpgradeTags);
		// TODO: Filter out Maxed upgrades or upgrades that have requirements. (This may be a future feature) 

		// TODO: Make number to draw not hardcoded. Make it a number that can be upgraded in game.
		//Draw 3 at random.
		int32 NumberToDraw = 3;
		for(int i = 0; i < NumberToDraw; i++)
		{
			if(ValidUpgradeTags.Num()<=0)
				break;
			int32 RandI = FMath::RandRange(0,ValidUpgradeTags.Num()-1);
			UpgradeState.UpgradeChoiceTags.Add(ValidUpgradeTags[RandI]);
			ValidUpgradeTags.RemoveAt(RandI);
		}
		

		
	}

	// TODO: Add a default card that does nothing, in the off chance this ever comes up naturally as a result of changes to how upgrades work.
	// Check if the player somehow doesnt have any choices.
	if(UpgradeState.UpgradeChoiceTags.Num()==0)
	{
		checkf(UpgradeState.UpgradeChoiceTags.Num()!=0, TEXT("Player somehow was given zero cards to choose from during levelup phase."));
	}

	
	Client_StartNewUpgradeState(UpgradeState);
}

void ASamPlayerState::HandleCharacterDeath(ASamCharacterBase* CharacterInstance)
{
	OnPlayerCharacterDeathDelegate.Broadcast(this);
	if(HasAuthority())
	{
		if(APlayerController* PC = GetPlayerController())
		{
			ASamPlayerController* SamPC = CastChecked<ASamPlayerController>(PC);
			SamPC->Auth_StartSpectating();
		}
	}
}

void ASamPlayerState::HandleCharacterRevive(ASamCharacterBase* CharacterInstance)
{
	OnPlayerCharacterReviveDelegate.Broadcast(this);
}

void ASamPlayerState::Client_StartNewUpgradeState_Implementation(FPlayerUpgradeState NewUpgradeState)
{
	UpgradeState = NewUpgradeState;
}

