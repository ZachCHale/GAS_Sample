// Copyright ZH_Dev


#include "Player/SamPlayerState.h"

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

void ASamPlayerState::AddToExp(int32 AddedExp)
{
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->AddToExp(AddedExp);
}

  
void ASamPlayerState::AddToLevel(int32 AddedLevels)  
{  
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	SamGS->AddToLevel(AddedLevels);
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
}

FPlayerUpgradeState* ASamPlayerState::GetPlayerUpgradeState()
{
	return &UpgradeState;
}

void ASamPlayerState::Auth_GenerateNewUpgradeSelection()
{
	
}

void ASamPlayerState::Auth_SubmitUpgradeSelection(FGameplayTag UpgradeTag)
{
	if(!HasAuthority())return;
	UpgradeState.bIsReady = true;
	UpgradeState.CurrentlySelectedChoice = UpgradeTag;
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	//TODO: Replace with delegate broadcast?
	SamGS->Auth_SendPlayerLevelUpSelection();
	
}

void ASamPlayerState::Auth_ClearUpgradeSelection()
{
	if(!HasAuthority())return;
	UpgradeState.bIsReady = false;
	UpgradeState.CurrentlySelectedChoice = FGameplayTag();
	ASamGameStateBase* SamGS = USamAbilitySystemLibrary::GetSamGameStateBase(this);
	//TODO: Replace with delegate broadcast?
	SamGS->Auth_ClearPlayerLevelUpSelection();
}

bool ASamPlayerState::IsUpgradeSelectionValid(FGameplayTag UpgradeTag)
{
	if(!UpgradeState.UpgradeChoiceTags.Contains(UpgradeTag)) return false;
	return true;
}

void ASamPlayerState::Server_StartNewUpgradeState_Implementation()
{
	UE_LOG(SamLog, Log, TEXT("Server"))
	ASamCharacterPlayer* PlayerCharacter = CastChecked<ASamCharacterPlayer>(AbilitySystemComponent->GetAvatarActor());
	UpgradeState.ResetSelectionState();
	//TODO: Remove Hardcoded number of upgrades to generate. Add ways to increase this number through gameplay.
	UpgradeState.UpgradeChoiceTags = PlayerCharacter->GetCharacterClassDefaultInfo().UpgradeSelectionInfo->GetRandomUpgradeTags(3);
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

void ASamPlayerState::Client_StartNewUpgradeState_Implementation(FPlayerUpgradeState NewUpgradeState)
{
	UpgradeState = NewUpgradeState;
	UE_LOG(SamLog, Log, TEXT("Client"))
}

