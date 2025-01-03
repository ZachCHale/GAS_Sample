// Copyright ZH_Dev

#include "Player/SamPlayerState.h"

#include "SamGameplayTags.h"
#include "SamGameStateBase.h"
#include "AbilitySystem/SamAbilitySystemComponent.h"
#include "AbilitySystem/SamAbilitySystemLibrary.h"
#include "AbilitySystem/SamAttributeSet.h"
#include "Character/SamCharacterPlayer.h"


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
	return UpgradeState.DrawnCardTags;
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
	USamAbilitySystemLibrary::CreateAndApplyGameplayEffectToSelf(this, FullHealGameplayEffect);
	ASamCharacterPlayer* PlayerCharacter = CastChecked<ASamCharacterPlayer>(AbilitySystemComponent->GetAvatarActor());
	PlayerCharacter->Auth_Revive();
	//Cast<ASamPlayerController>(GetPlayerController())->Auth_StopSpectating();
}

void ASamPlayerState::Auth_SubmitUpgradeSelection(FGameplayTag UpgradeTag)
{
	if(!HasAuthority())return;
	UpgradeState.bIsReady = true;
	UpgradeState.CurrentlySelectedCardTag = UpgradeTag;
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	OnAuthUpgradeStateChangedDelegate.Broadcast(this);
}

void ASamPlayerState::Auth_ClearUpgradeSelection()
{
	if(!HasAuthority())return;
	UpgradeState.bIsReady = false;
	UpgradeState.CurrentlySelectedCardTag = FGameplayTag();
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
	if(!UpgradeState.DrawnCardTags.Contains(UpgradeTag)) return false;
	return true;
}

void ASamPlayerState::Server_StartNewUpgradeState_Implementation()
{
	ASamCharacterPlayer* PlayerCharacter = CastChecked<ASamCharacterPlayer>(AbilitySystemComponent->GetAvatarActor());
	UpgradeState.ResetSelectionState();
	if(!HasLivingCharacter())
	{
		//TODO: Make sure there are no bugs related to reviving while the character is in the process of dying (hasn't finished fading out and hasn't switched to spectator)
		UpgradeState.DrawnCardTags.Add(SamTags::Cards::Card_Special_Revive);
	}
	else
	{
		USamAbilitySystemComponent* SamASC = CastChecked<USamAbilitySystemComponent>(GetAbilitySystemComponent());
		// Upgrade Tags
		TArray<FGameplayTag> ValidUpgradeTags;
		TArray<FGameplayTag> FilteredUpgradeTags;
		// Get all upgrades available for this class
		PlayerCharacter->GetCharacterClassDefaultInfo().AvailableUpgradeCards.GetGameplayTagArray(ValidUpgradeTags);
		UUpgradeCardDatabase* UpgradeCardDatabase = USamAbilitySystemLibrary::GetCardDatabase(this)->UpgradeCardDatabase;
		for (auto ValidUpgradeTag : ValidUpgradeTags)
		{
			bool bIsUpgradeValid = true;
			int32 CurrentLevel = SamASC->GetCurrentUpgradeLevel(ValidUpgradeTag);
			if(UpgradeCardDatabase->GetCapForUpgrade(ValidUpgradeTag) <= CurrentLevel)
			{
				bIsUpgradeValid = false;
			}
			if(bIsUpgradeValid)
				FilteredUpgradeTags.Add(ValidUpgradeTag);
		}
		
		// TODO: Make number to draw not hardcoded. Make it a number that can be upgraded in game.
		//Draw 3 at random.
		int32 NumberToDraw = 3;
		for(int i = 0; i < NumberToDraw; i++)
		{
			if(FilteredUpgradeTags.Num()<=0)
				break;
			int32 RandI = FMath::RandRange(0,FilteredUpgradeTags.Num()-1);
			UpgradeState.DrawnCardTags.Add(FilteredUpgradeTags[RandI]);
			FilteredUpgradeTags.RemoveAt(RandI);
		}
		

		
	}
	
	// Check if the player somehow doesn't have any choices.
	if(UpgradeState.DrawnCardTags.Num()==0)
	{
		UpgradeState.DrawnCardTags.Add(SamTags::Cards::Card_Special_Nothing);
	}
	
	Client_StartNewUpgradeState(UpgradeState);
}

void ASamPlayerState::HandleCharacterDeath(ASamCharacterBase* CharacterInstance)
{
	OnPlayerCharacterDeathDelegate.Broadcast(this);
}

void ASamPlayerState::HandleCharacterRevive(ASamCharacterBase* CharacterInstance)
{
	OnPlayerCharacterReviveDelegate.Broadcast(this);
}

void ASamPlayerState::Client_StartNewUpgradeState_Implementation(FPlayerUpgradeState NewUpgradeState)
{
	UpgradeState = NewUpgradeState;
}

