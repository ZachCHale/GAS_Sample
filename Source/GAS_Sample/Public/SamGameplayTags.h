// Copyright ZH_Dev
#pragma once

#include "CoreMinimal.h"  
#include "NativeGameplayTags.h"

//Attribute Tags
namespace SamTags::AttributeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Vital_MaxHealth);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Vital_Health);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_MovementSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Primary_Block);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Attack_Damage)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Attack_AttackSpeed)

	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Resistance_Physical);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Resistance_Magic);

}

//Input Binding Tags
//Used for binding input actions to abilities for player controlled characters
namespace SamTags::InputBindingTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputBinding_SlottedAbility_1);
}

//Ability Tags
//Added to abilities
namespace SamTags::AbilityTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityTag_ActivateOnDeath);
}

//Caller Magnitude Tags
//Used for effects whose magnitudes are set by the caller. Usually named after a meta attribute.
namespace SamTags::CallerMagnitudeTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CallerMagnitude_IncomingExp);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CallerMagnitude_IncomingDamage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(CallerMagnitude_AbilityCooldown);
}

//Damage Type Tags
//Represents categories of damage. Used as CallerMagnitudes within ExecCalc_Damage
namespace SamTags::DamageTypes
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageType_Physical);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageType_Magic);
}

namespace SamTags::Cooldown
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_PrimaryAttack);
}

namespace SamTags::Mappings
{
	TMap<FGameplayTag, FGameplayTag> GetDamageTypesToResistancesMap();
}

