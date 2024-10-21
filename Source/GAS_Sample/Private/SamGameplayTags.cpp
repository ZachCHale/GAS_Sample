// Copyright ZH_Dev

#include "SamGameplayTags.h"  

namespace SamTags::AttributeTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MovementSpeed, "Attribute.Primary.MovementSpeed","Character Movement Speed")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_MaxHealth, "Attribute.Vital.MaxHealth","Character Max Health")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_Health, "Attribute.Vital.Health","Character Current Health")
}

namespace SamTags::InputBindingTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputBinding_SlottedAbility_1, "InputBinding.SlottedAbility.1","Input activates the first slotted ability")
}

namespace SamTags::AbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityTag_ActivateOnDeath, "AbilityTag.ActivateOnDeath","Ability should activate when health reaches zero.")
}

namespace SamTags::CallerMagnitudeTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CallerMagnitude_IncomingExp, "CallerMagnitude.IncomingExp","Caller Magnitude for Effects that grant Exp")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CallerMagnitude_IncomingDamage, "CallerMagnitude.IncomingDamage","Caller Magnitude for Effects that apply damage")
}