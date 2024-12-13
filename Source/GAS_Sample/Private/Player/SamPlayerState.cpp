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
	if(HasAuthority())
		PlayerStateList.Remove(this);
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

