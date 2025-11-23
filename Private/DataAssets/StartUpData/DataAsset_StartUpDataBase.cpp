// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

/**
 * @brief 将启动数据应用到能力系统组件实现
 * 
 * 将数据资产中配置的能力和效果应用到指定的能力系统组件
 * 这是启动数据资产的核心功能函数
 * 
 * @param InASCToGive 指向要应用数据的能力系统组件指针
 * @param ApplyLevel 应用等级，默认为1级
 * 
 * @details
 * 1. 验证能力系统组件指针的有效性
 * 2. 授予激活类能力和反应类能力
 * 3. 应用启动时的游戏效果
 */
void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
	int32 ApplyLevel)
{
	// 检查能力系统组件指针的有效性，无效则触发断言
	check(InASCToGive);

	// 授予激活时的能力
	// 将ActivateOnGivenAbilities数组中的能力授予给指定的能力系统组件
	GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
	
	// 授予反应类能力
	// 将ReactiveAbilities数组中的能力授予给指定的能力系统组件
	GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);

	// 将一组启动时的GameplayEffect应用到指定的AbilitySystemComponent上
	// 检查启动游戏效果数组是否为空
	if (!StartUpGameplayEffects.IsEmpty())
	{
		// 遍历启动游戏效果数组
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			// 检查效果类是否有效，无效则跳过
			if (!EffectClass) continue;
			
			// 创建一个GameplayEffectSpecHandle实例，使用指定的EffectClass和ApplyLevel
			// MakeOutgoingSpec用于创建一个效果规范句柄，包含效果类、等级和效果上下文
			FGameplayEffectSpecHandle EffectSpecHandle = InASCToGive->MakeOutgoingSpec(EffectClass, ApplyLevel, InASCToGive->MakeEffectContext());
			
			// 检查效果规范句柄是否有效
			if (EffectSpecHandle.IsValid())
			{
				// 应用效果规范到自身，而不是直接使用EffectCDO
				// 这样可以确保所有属性都被正确初始化和处理
				// 添加空指针检查以防止访问空指针导致的断言失败
				// if (EffectSpecHandle.Data.IsValid() && EffectSpecHandle.Data.Get())
				// {
				// 	InASCToGive->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
				// }
				
				// 通过 EffectClass（一个 UGameplayEffect 的子类类型），获取该类的默认对象（CDO，Class Default Object）
				// 即该效果的模板实例
				UGameplayEffect* EffectCDO = EffectClass ->GetDefaultObject<UGameplayEffect>();
				
				// 检查效果默认对象是否有效
				if (EffectCDO)
				{
					// 调用能力系统组件的 ApplyGameplayEffectToSelf 方法
					// 将上一步获取到的 GameplayEffect 默认对象以 ApplyLevel 等级和指定的效果上下文（EffectContext）应用到自己身上
					InASCToGive->ApplyGameplayEffectToSelf(
						EffectCDO,
						ApplyLevel,
						InASCToGive->MakeEffectContext()
					 );
				}
				
				/* 以下为原始实现方式（已废弃）：
				 * // 通过 EffectClass（一个 UGameplayEffect 的子类类型），获取该类的默认对象（CDO，Class Default Object）
				 * // 即该效果的模板实例
				 * UGameplayEffect* EffectCDO = EffectClass ->GetDefaultObject<UGameplayEffect>();
				 * // 调用能力系统组件的 ApplyGameplayEffectToSelf 方法
				 * // 将上一步获取到的 GameplayEffect 默认对象以 ApplyLevel 等级和指定的效果上下文（EffectContext）应用到自己身上
				 * InASCToGive->ApplyGameplayEffectToSelf(
				 *     EffectCDO,
				 *     ApplyLevel,
				 *     InASCToGive->MakeEffectContext()
				 * );
				 */
			}
		}
	}
}

/**
 * @brief 授予能力的辅助函数实现
 * 
 * 将指定数组中的能力授予给指定的能力系统组件
 * 
 * @param InAbilitiesToGive 要授予的能力类数组
 * @param InASCToGive 目标能力系统组件指针
 * * @param ApplyLevel 能力应用等级，默认为1级
 * 
 * @details
 * 1. 检查能力数组是否为空
 * 2. 遍历能力数组，为每个有效能力创建能力规范
 * 3. 设置能力规范的源对象和等级
 * 4. 将能力规范授予给能力系统组件
 */
void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive,
	UWarriorAbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	// 检查要授予的能力数组是否为空，为空则直接返回
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	// 遍历要授予的能力数组
	for (const TSubclassOf<UWarriorGameplayAbility>& Ability : InAbilitiesToGive)
	{
		// 检查能力类是否有效，无效则跳过
		if (!Ability) continue;

		// 创建能力规范，使用能力类作为参数
		FGameplayAbilitySpec AbilitySpec(Ability);
		
		// 设置能力规范的源对象为能力系统组件的化身演员
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		
		// 设置能力规范的等级
		AbilitySpec.Level = ApplyLevel;
		
		// 将能力规范授予给能力系统组件
		// GiveAbility会将能力添加到能力系统组件中，使其可以被激活和使用
		InASCToGive->GiveAbility(AbilitySpec);
	}
}