// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GE_ExecCalc/GE_ExecCalc_DamageTaken.h"

#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAttributeSet.h"

/**
 * @brief 用于定义和捕获与伤害计算相关的属性的结构体
 * 
 * 该结构体封装了伤害计算所需的所有属性捕获定义，包括攻击方的攻击力、
 * 防御方的防御力以及最终伤害值。使用静态实例确保只创建一次，提高性能。
 * 
 * @details
 * 1. DECLARE_ATTRIBUTE_CAPTUREDEF宏声明属性捕获定义
 * 2. DEFINE_ATTRIBUTE_CAPTUREDEF宏定义具体的属性捕获参数
 * 3. 包含了计算伤害所需的所有相关属性定义
 */
struct FWarriorDamageCapture
{
	// 声明攻击力属性捕获定义
	// 用于捕获攻击方的攻击力属性
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	
	// 声明防御力属性捕获定义
	// 用于捕获防御方的防御力属性
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	
	// 声明伤害承受属性捕获定义
	// 用于捕获最终计算出的伤害值属性
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	/**
	 * @brief 构造函数，定义属性捕获的具体参数
	 * 
	 * 使用宏来简化属性捕获定义的过程，为每个属性设置捕获源和捕获时机
	 * 
	 * @details
	 * 1. AttackPower从Source（攻击方）获取，不在执行时捕获
	 * 2. DefensePower从Target（防御方）获取，不在执行时捕获
	 * 3. DamageTaken从Target（防御方）获取，不在执行时捕获
	 * 4. 所有属性都使用UWarriorAttributeSet作为属性集类
	 */
	FWarriorDamageCapture()
	{
		// 定义攻击力属性捕获：从Source（攻击方）获取AttackPower属性，不在执行时捕获
		// 参数说明：属性集类、属性名、捕获源(Source/Target)、是否在执行时捕获
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false)
		
		// 定义防御力属性捕获：从Target（防御方）获取DefensePower属性，不在执行时捕获
		// 参数说明：属性集类、属性名、捕获源(Source/Target)、是否在执行时捕获
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false)
		
		// 定义伤害承受属性捕获：从Target（防御方）获取DamageTaken属性，不在执行时捕获
		// 参数说明：属性集类、属性名、捕获源(Source/Target)、是否在执行时捕获
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false)
	}
};

/**
 * @brief 获取静态的FWarriorDamageCapture实例
 * 
 * 使用局部静态变量确保只创建一次实例，提高性能并保证线程安全
 * 这是C++中单例模式的一种实现方式
 * 
 * @return 返回FWarriorDamageCapture的引用
 * 
 * @details
 * 1. 使用局部静态变量确保延迟初始化
 * 2. 保证线程安全性（C++11标准保证）
 * 3. 避免多次创建和销毁对象的开销
 */
static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	// 局部静态变量，C++11标准保证线程安全的延迟初始化
	// 只在第一次调用时创建实例，后续调用直接返回已创建的实例
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

/**
 * @brief 构造函数，初始化游戏效果执行计算所需的属性捕获定义
 * 
 * 将所有需要捕获的属性定义添加到相关属性列表中，供执行计算时使用
 * 这里使用了优化的方法，通过静态函数获取预定义的捕获定义
 * 
 * @details
 * 1. 添加攻击力属性捕获定义到相关属性列表中
 * 2. 添加防御力属性捕获定义到相关属性列表中
 * 3. 添加伤害承受属性捕获定义到相关属性列表中
 * 4. 这些属性将在执行计算时被使用
 */
UGE_ExecCalc_DamageTaken::UGE_ExecCalc_DamageTaken()
{
	// 添加攻击力属性捕获定义到相关属性列表中
	// 该属性将用于获取攻击方的攻击力数值
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	
	// 添加防御力属性捕获定义到相关属性列表中
	// 该属性将用于获取防御方的防御力数值
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);

	// 添加伤害承受属性捕获定义到相关属性列表中
	// 该属性将用于设置最终计算出的伤害值
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

/**
 * @brief 执行计算实现函数，这是游戏效果执行时调用的核心函数
 * 
 * 用于计算伤害值并输出结果，是整个伤害计算逻辑的核心实现
 * 
 * @param ExecutionParams 执行参数，包含源和目标的信息，如属性值、标签等
 * @param OutExecutionOutput 执行输出，用于输出计算结果，如修改后的属性值
 * 
 * @details
 * 1. 获取效果规格说明和相关参数
 * 2. 创建聚合评估参数用于属性值计算
 * 3. 获取攻击方攻击力和防御方防御力
 * 4. 解析调用者设置的基础伤害和攻击类型信息
 * 5. 根据攻击类型和连击次数计算伤害增幅
 * 6. 应用伤害计算公式得出最终伤害值
 * 7. 将最终伤害值作为输出修饰符添加到执行输出中
 */
void UGE_ExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 获取拥有此效果的规格说明，包含所有效果相关的信息
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// EffectSpec.GetContext().GetSourceObject();
	// EffectSpec.GetContext().GetAbility();
	// EffectSpec.GetContext().GetInstigator();
	// EffectSpec.GetContext().GetEffectCauser();
	
	// 创建聚合评估参数，用于属性值的计算
	// 这些参数将用于获取捕获的属性值
	FAggregatorEvaluateParameters EvaluationParameters;
	
	// 设置源标签，用于属性捕获时的标签匹配
	EvaluationParameters.SourceTags =  EffectSpec.CapturedSourceTags.GetAggregatedTags();
	
	// 设置目标标签，用于属性捕获时的标签匹配
	EvaluationParameters.TargetTags =  EffectSpec.CapturedTargetTags.GetAggregatedTags();

	// 获取源的攻击力数值
	// 通过AttemptCalculateCapturedAttributeMagnitude方法计算并获取攻击方的攻击力
	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluationParameters, SourceAttackPower);

	// Debug::Print(TEXT("SourceAttackPower"), SourceAttackPower);
	
	// 初始化基础伤害值和攻击连击计数
	// 这些值将从调用者设置的标签数值中获取
	float BaseDamage = 0.f;
	int32 UsedLightAttackComboCount = 0;
	int32 UsedHeavyAttackComboCount = 0;
	
	// 遍历所有由调用者设置的标签数值，获取基础伤害和攻击类型信息
	// SetByCallerTagMagnitudes允许在应用效果时动态设置属性值
	for (const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		// 检查是否为基本伤害标签
		// Shared_SetByCaller_BaseDamage标签用于标识基础伤害值
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			// 设置基础伤害值
			// 从标签对应的数值中获取基础伤害
			BaseDamage = TagMagnitude.Value;
			
			// Debug::Print(TEXT("BaseDamage"), BaseDamage);
		}

		// 检查是否为轻攻击类型标签
		// Player_SetByCaller_AttackType_Light标签用于标识轻攻击及其连击次数
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			// 设置轻攻击连击次数
			// 数值表示当前轻攻击的连击计数
			UsedLightAttackComboCount = TagMagnitude.Value;
			
			// Debug::Print(TEXT("UsedLightAttackComboCount"), UsedLightAttackComboCount);
		}
		// 检查是否为重攻击类型标签
		// Player_SetByCaller_AttackType_Heavy标签用于标识重攻击及其连击次数
		else if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			// 设置重攻击连击次数
			// 数值表示当前重攻击的连击计数
			UsedHeavyAttackComboCount = TagMagnitude.Value;
			
			// Debug::Print(TEXT("UsedHeavyAttackComboCount"), UsedHeavyAttackComboCount);
		}
	}
		
	// 获取目标的防御力数值
	// 通过AttemptCalculateCapturedAttributeMagnitude方法计算并获取防御方的防御力
	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef, EvaluationParameters, TargetDefensePower);

	// Debug::Print(TEXT("TargetDefensePower"), TargetDefensePower);
	
	// 如果有轻攻击连击，则计算伤害增加百分比
	// 轻攻击连击会提供递增的伤害加成
	if (UsedLightAttackComboCount != 0)
	{
		// 计算轻攻击连击伤害增幅：每连击一次增加5%伤害，第一次连击为基础100%
		// 公式：伤害倍率 = (连击次数 - 1) * 0.05 + 1.0
		const float DamageIncreasePercentLight = (UsedLightAttackComboCount - 1) * 0.05f + 1.f;
		
		// 应用轻攻击连击伤害增幅到基础伤害
		BaseDamage *= DamageIncreasePercentLight;
		
		// Debug::Print(TEXT("ScaledBaseDamageLight"), BaseDamage);
	}

	// 如果有重攻击连击，则计算伤害增加百分比
	// 重攻击连击会提供比轻攻击更高的递增伤害加成
	if (UsedHeavyAttackComboCount != 0)
	{
		// 计算重攻击连击伤害增幅：每连击一次增加15%伤害，第一次连击为基础100%
		// 公式：伤害倍率 = (连击次数 - 1) * 0.15 + 1.0
		const float DamageIncreasePercentHeavy = (UsedHeavyAttackComboCount - 1) * 0.15f + 1.f;
		
		// 应用重攻击连击伤害增幅到基础伤害
		BaseDamage *= DamageIncreasePercentHeavy;
		
		// Debug::Print(TEXT("ScaledBaseDamageHeavy"), BaseDamage);
	}
	
	// 计算最终伤害值
	// 伤害公式：最终伤害 = 基础伤害 * 攻击方攻击力 / 防御方防御力
	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	
	// 调试输出最终伤害值
	// Debug::Print(TEXT("FinalDamageDone"), FinalDamageDone);

	// 如果最终伤害值大于0，则将其作为修饰符添加到执行输出中
	// 只有正伤害才会被应用到目标角色
	if (FinalDamageDone > 0.f)
	{
		// 添加输出修饰符，将计算出的伤害值应用到目标的DamageTaken属性上
		// 使用Override操作覆盖目标属性值
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UWarriorAttributeSet::GetDamageTakenAttribute(),
		EGameplayModOp::Override, FinalDamageDone));
	}

	
	
}