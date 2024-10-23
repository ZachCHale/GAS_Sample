// Copyright ZH_Dev


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "SamLogChannels.h"

struct SamDamageStatics
{
	// Source
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageScale)
	// Target
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Block)
	
	SamDamageStatics()
	{
		//Source
		DEFINE_ATTRIBUTE_CAPTUREDEF(USamAttributeSet, DamageScale, Source, true);
		
		//Target
		DEFINE_ATTRIBUTE_CAPTUREDEF(USamAttributeSet, PhysicalResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USamAttributeSet, MagicResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USamAttributeSet, Block, Target, false);

	}
	FGameplayEffectAttributeCaptureDefinition& GetCaptureDefFromTag(const FGameplayTag& Tag) const
	{
		static TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDef;
		if(TagsToCaptureDef.IsEmpty())
		{
			TagsToCaptureDef.Add(SamTags::AttributeTags::Attribute_Attack_Damage, DamageScaleDef);
			TagsToCaptureDef.Add(SamTags::AttributeTags::Attribute_Resistance_Physical, PhysicalResistanceDef);
			TagsToCaptureDef.Add(SamTags::AttributeTags::Attribute_Resistance_Magic, MagicResistanceDef);
			TagsToCaptureDef.Add(SamTags::AttributeTags::Attribute_Primary_Block, BlockDef);
		}
		checkf(TagsToCaptureDef.Contains(Tag), TEXT("TagsToCaptureDef doesnt contain [%s] in ExecCalc_Damage"), *Tag.ToString());
		return TagsToCaptureDef[Tag];
	}
};

static const SamDamageStatics& DamageStatics()
{
	static SamDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageScaleDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle Context = Spec.GetContext();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float FinalDamage = 0.f;

	for (auto[DamageType, ResistanceType] : SamTags::Mappings::GetDamageTypesToResistancesMap())
	{
		float CMag_DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageType, false, 0);

		float Tgt_ResistanceTypeValue = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().GetCaptureDefFromTag(ResistanceType), EvalParams, Tgt_ResistanceTypeValue);
		Tgt_ResistanceTypeValue = FMath::Clamp<float>(Tgt_ResistanceTypeValue, 0.f, 1.f);
		FinalDamage += CMag_DamageTypeValue * (1-Tgt_ResistanceTypeValue);
	}
	
	float Src_DamageScale = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageScaleDef, EvalParams, Src_DamageScale);
	
	FinalDamage *= Src_DamageScale;

	float Tgt_Block = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockDef, EvalParams, Tgt_Block);
	Tgt_Block = FMath::Max<float>(0.f, Tgt_Block);
	FinalDamage -= Tgt_Block;
	
	FinalDamage = FMath::Max<float>(0.f, FinalDamage);
	
	const FGameplayModifierEvaluatedData EvaluatedData(USamAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
