// Copyright ZH_Dev

#include "SamGameplayTags.h"  

namespace SamTags::AttributeTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Primary_MovementSpeed, "Attribute.Primary.MovementSpeed","Character Movement Speed")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_MaxHealth, "Attribute.Vital.MaxHealth","Character Max Health")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Vital_Health, "Attribute.Vital.Health","Character Current Health")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack_Damage, "Attribute.Attack.Damage", "Damage Scaling for attacks")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Resistance_Physical, "Attribute.Resistance.Physical", "Resistance to Physical attacks")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Resistance_Magic, "Attribute.Attack.Magic", "Resistance to Magic attacks")

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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CallerMagnitude_IncomingDamage, "CallerMagnitude.IncomingDamage","Caller Magnitude for Effects that apply damage. Used in ExecCalc_Damage. Exec Calc will look at Damage Tags set by caller.")
}

namespace SamTags::DamageTypes
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageType_Physical, "DamageType.Physical","Physical Damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageType_Magic, "DamageType.Magic","Magic Damage")
}

namespace SamTags::Mappings
{
	TMap<FGameplayTag, FGameplayTag> SamTags::Mappings::GetDamageTypesToResistancesMap()
	{
		static TMap<FGameplayTag, FGameplayTag> DamageToResist;
		if(DamageToResist.IsEmpty())
		{
			DamageToResist.Add(SamTags::DamageTypes::DamageType_Physical, SamTags::AttributeTags::Attribute_Resistance_Physical);
			DamageToResist.Add(SamTags::DamageTypes::DamageType_Magic, SamTags::AttributeTags::Attribute_Resistance_Magic);
		}
		return DamageToResist;
	}
}

