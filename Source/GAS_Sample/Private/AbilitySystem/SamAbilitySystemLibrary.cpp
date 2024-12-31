// Copyright ZH_Dev


#include "AbilitySystem/SamAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "SamGameModeBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SamGameStateBase.h"
#include "SamLogChannels.h"
#include "AbilitySystem/SamGameplayAbility.h"
#include "Actor/Interface/TeamInterface.h"
#include "Character/SamCharacterPlayer.h"
#include "GameFramework/Character.h"
#include "Player/SamPlayerState.h"
#include "UI/SamWidgetController.h"
#include "UI/HUD/SamHUD.h"

UCharacterClassInfo* USamAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	ASamGameStateBase* SamGS = GetSamGameStateBase(WorldContextObject);
	
	if(SamGS == nullptr) return nullptr;
	return SamGS->CharacterClassInfo;
}

ULevelSpawnPatternInfo* USamAbilitySystemLibrary::GetLevelSpawnPatternInfo(const UObject* WorldContextObject)
{
	ASamGameModeBase* SamGameMode = Cast<ASamGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(SamGameMode == nullptr) return nullptr;
	return SamGameMode->LevelSpawnPatternInfo;
}

FCharacterClassDefaultInfo USamAbilitySystemLibrary::GetDefaultInfoForCharacterClass(const UObject* WorldContextObject,
                                                                                     const FGameplayTag CharacterClassTag)
{
	return GetCharacterClassInfo(WorldContextObject)->GetClassDefaultInfoFromTag(CharacterClassTag);
}

void USamAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, FGameplayTag CharacterClassTag, UAbilitySystemComponent* ASC, int32 Level)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(ClassInfo == nullptr) return;
	FCharacterClassDefaultInfo DefaultInfo = ClassInfo->ClassDefaultInfo[CharacterClassTag];

	AActor* AvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle DefaultAttributesContextHandle = ASC->MakeEffectContext();
	DefaultAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle DefaultAttributesSpecHandle = ASC->MakeOutgoingSpec(DefaultInfo.DefaultAttributes, Level, DefaultAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*DefaultAttributesSpecHandle.Data.Get());
}

void USamAbilitySystemLibrary::InitializeDefaultAbilities(const UObject* WorldContextObject,
	FGameplayTag CharacterClassTag, UAbilitySystemComponent* ASC, int32 Level)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(ClassInfo == nullptr) return;
	FCharacterClassDefaultInfo DefaultInfo = ClassInfo->ClassDefaultInfo[CharacterClassTag];

	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilityClasses = DefaultInfo.DefaultAbilities;

	for (auto AbilityClass : DefaultAbilityClasses)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, Level);
		if(USamGameplayAbility* SamAbility = Cast<USamGameplayAbility>(AbilitySpec.Ability) )
		{
			AbilitySpec.DynamicAbilityTags.AddTag(SamAbility->StartupTag);
		}
		ASC->GiveAbility(AbilitySpec);
	}
}
//Use this for applying from objects that don't have an ASC.
bool USamAbilitySystemLibrary::CreateAndApplyGameplayEffectToSelf(AActor* Target,
	TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 Level)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if(TargetASC == nullptr)
		return false;
	AActor* Instigator = TargetASC->GetOwnerActor();
	AActor* EffectCauser = Instigator;
	UObject* SourceObject = TargetASC->GetAvatarActor();
	
	FGameplayEffectSpecHandle SpecHandle = CreateGameplayEffectSpecHandle(GameplayEffectClass, Instigator, EffectCauser, SourceObject, Level);
	return ApplyGameplayEffectSpecToTarget(Target, SpecHandle);
}

bool USamAbilitySystemLibrary::CreateAndApplyGameplayEffectToTarget(AActor* Target,
                                                                    TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* Instigator, AActor* EffectCauser, UObject* SourceObject,
                                                                    int32 Level)
{
	FGameplayEffectSpecHandle SpecHandle = CreateGameplayEffectSpecHandle(GameplayEffectClass, Instigator, EffectCauser, SourceObject, Level);
	return ApplyGameplayEffectSpecToTarget(Target, SpecHandle);
}

FGameplayEffectSpecHandle USamAbilitySystemLibrary::CreateGameplayEffectSpecHandle(
	TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* Instigator, AActor* EffectCauser, UObject* SourceObject,
	int32 Level)
{
	check(GameplayEffectClass);
	check(Instigator);
	check(EffectCauser);
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Instigator);
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddInstigator(Instigator,EffectCauser);
	ContextHandle.AddSourceObject(SourceObject);
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	return SpecHandle;
}

bool USamAbilitySystemLibrary::ApplyGameplayEffectSpecToTarget(AActor* Target, FGameplayEffectSpecHandle SpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if(TargetASC == nullptr)
		return false;
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}


UOverlayWidgetController* USamAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(ASamHUD* SamHUD = Cast<ASamHUD>(PC->GetHUD()))
		{
			ASamPlayerState* PS = PC->GetPlayerState<ASamPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return SamHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

TArray<FVector> USamAbilitySystemLibrary::GetCurrentPlayerCharacterLocations(const UObject* WorldContextObject)
{
	static double LastQueryTime = -1;
	static TArray<FVector> CurrentLocations;
	if(LastQueryTime == UGameplayStatics::GetTimeSeconds(WorldContextObject))
		return CurrentLocations;
	LastQueryTime = UGameplayStatics::GetTimeSeconds(WorldContextObject);
	CurrentLocations.Empty();
	ASamGameStateBase* SamGS = GetSamGameStateBase(WorldContextObject);
	TArray<ACharacter*> PlayerCharacters = GetLivePlayerCharacters(WorldContextObject);
	for (ACharacter* PlayerCharacter : PlayerCharacters)
	{
		CurrentLocations.Add(PlayerCharacter->GetActorLocation());
	}
	return CurrentLocations;
}

TArray<ACharacter*> USamAbilitySystemLibrary::GetLivePlayerCharacters(const UObject* WorldContextObject)
{
	ASamGameStateBase* SamGS = GetSamGameStateBase(WorldContextObject);
	TArray<ACharacter*> PlayerCharacters = SamGS->GetAllPlayerCharacters();
	TArray<ACharacter*> LiveCharacters;
	for (ACharacter* PlayerCharacter : PlayerCharacters)
	{
		ASamCharacterBase* SamCharacter = CastChecked<ASamCharacterBase>(PlayerCharacter);
		if(!SamCharacter->GetIsDead())
			LiveCharacters.Add(SamCharacter);
	}
	return LiveCharacters;
}

ASamGameStateBase* USamAbilitySystemLibrary::GetSamGameStateBase(const UObject* WorldContextObject)
{
	UWorld* World = WorldContextObject->GetWorld();
	check(World)
	AGameStateBase* GS = World->GetGameState();
	if(GS == nullptr)
	{
		UE_LOG(SamLog, Error, TEXT("GameStateWasNull"))
		return nullptr;
	}
	return CastChecked<ASamGameStateBase>(GS);
}

UAbilitySystemComponent* USamAbilitySystemLibrary::GetLocalASC(const UObject* WorldContextObject)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		ASamPlayerState* SamPS = PC->GetPlayerState<ASamPlayerState>();
		return SamPS->GetAbilitySystemComponent();
	}
	return nullptr;
}




