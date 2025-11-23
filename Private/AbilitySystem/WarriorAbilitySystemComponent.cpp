// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"

#include "WarriorGameplayTags.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

/**
 * @brief 处理能力输入按下的函数
 * 
 * 当玩家按下与特定GameplayTag关联的输入时调用此函数
 * 遍历所有可激活的能力，查找与输入标签匹配的能力并尝试激活
 * 
 * @param InInputTag 与按下输入相关联的GameplayTag，用于匹配对应的能力
 * 
 * @details
 * 1. 首先验证输入标签的有效性
 * 2. 遍历所有可激活的能力规格
 * 3. 检查能力的动态标签是否包含指定的输入标签
 * 4. 如果匹配，则尝试激活该能力
 * 
 * @note 使用GetDynamicSpecSourceTags()替代已弃用的DynamicAbilityTags()方法
 */
void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	// 检查输入标签是否有效，无效则直接返回
	if (!InInputTag.IsValid())
	{
		return; 
	}

	// 遍历所有可激活的能力规格
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 原为 if (!AbilitySpec.DynamicAbilityTags().HasTagExact(InInputTag)) continue;
		// 检查能力规格的动态标签是否精确匹配输入标签，不匹配则继续下一个能力
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		if (InInputTag.MatchesTag(WarriorGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
			TryActivateAbility(AbilitySpec.Handle);
		}

		
	}
}

/**
 * @brief 处理能力输入释放的函数
 * 
 * 当玩家释放与特定GameplayTag关联的输入时调用此函数
 * 目前为空实现，待后续完善
 * 
 * @param InInputTag 与释放输入相关联的GameplayTag
 * 
 * @details
 * 该函数用于处理能力的输入释放逻辑，例如:
 * 1. 取消持续性能力
 * 2. 触发释放相关的效果
 * 3. 更新能力状态
 * 
 * @todo 实现完整的输入释放逻辑
 */

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(WarriorGameplayTags::InputTag_MustBeHeld))
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 修改此处逻辑，只取消那些明确需要释放事件的能力
		// 对于普通点击触发的攻击动画等能力，不应在此处被取消
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			// 检查能力是否真的需要在输入释放时取消
			// 只有带有InputTag_MustBeHeld标签的能力才会在输入释放时被取消
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(WarriorGameplayTags::InputTag_MustBeHeld) ||
				InInputTag.MatchesTag(WarriorGameplayTags::InputTag_MustBeHeld_Block))
			{
				CancelAbilityHandle(AbilitySpec.Handle);
			}
		}
	}

	
}

/**
 * @brief 授予英雄武器能力的函数
 * 
 * 根据传入的武器能力集合，为角色授予相应的游戏能力
 * 这些能力通常与特定武器相关联，在装备武器时调用
 * 
 * @param InDefaultWeaponAbilities 要授予的武器能力集合，包含输入标签和能力类的映射关系
 * @param ApplyLevel 能力应用的等级，影响能力的效果强度
 * @param OutGrantedAbilitySpecHandles 输出参数，返回授予的能力规格句柄数组，用于后续管理这些能力
 * 
 * @details
 * 1. 检查传入的能力集合是否为空
 * 2. 遍历能力集合中的每个能力配置
 * 3. 验证能力配置的有效性
 * 4. 为每个有效能力创建能力规格实例
 * 5. 设置能力的源对象、等级和动态标签
 * 6. 授予能力并保存能力句柄
 * 
 * @note 使用GetDynamicSpecSourceTags()替代已弃用的DynamicAbilityTags()方法
 */
void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(
	const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,
	int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	// 检查传入的武器能力集合是否为空，为空则直接返回
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return;
	}

	// 遍历所有要授予的武器能力集合
	for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		// 验证当前能力配置是否有效，无效则跳过
		if (!AbilitySet.IsValid())
		{
			continue;
		}
		
		// 根据能力集中的能力创建能力规范实例
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		
		// 设置能力的源对象为角色的Avatar Actor
		// 源对象通常用于效果的上下文信息
		AbilitySpec.SourceObject = GetAvatarActor();
		
		// 设置能力等级为传入的应用等级
		// 等级会影响能力的效果强度和属性值
		AbilitySpec.Level = ApplyLevel;

		// 原本使用的FGameplayAbilitySpec::DynamicAbilityTags已被弃用
		// 将能力集中的输入标签添加到能力的动态标签中
		// 动态标签用于在运行时匹配输入事件
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
	
		// 授予能力并获取能力规格句柄
		// 使用AddUnique确保不会重复添加相同的句柄
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
	
	// 遍历所有要授予的武器能力集合
	for (const FWarriorHeroSpecialAbilitySet& AbilitySet : InSpecialWeaponAbilities)
	{
		// 验证当前能力配置是否有效，无效则跳过
		if (!AbilitySet.IsValid())
		{
			continue;
		}
		
		// 根据能力集中的能力创建能力规范实例
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		
		// 设置能力的源对象为角色的Avatar Actor
		// 源对象通常用于效果的上下文信息
		AbilitySpec.SourceObject = GetAvatarActor();
		
		// 设置能力等级为传入的应用等级
		// 等级会影响能力的效果强度和属性值
		AbilitySpec.Level = ApplyLevel;

		// 原本使用的FGameplayAbilitySpec::DynamicAbilityTags已被弃用
		// 将能力集中的输入标签添加到能力的动态标签中
		// 动态标签用于在运行时匹配输入事件
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
	
		// 授予能力并获取能力规格句柄
		// 使用AddUnique确保不会重复添加相同的句柄
		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
	
}

/**
 * @brief 移除已授予的英雄武器能力
 * 
 * 清除之前授予的角色武器能力，通常在卸载武器时调用
 * 
 * @param InSpecHandlesToRemove 要移除的能力规格句柄数组的引用
 * 通过引用传递允许函数修改外部数组
 * 
 * @details
 * 1. 检查要移除的能力句柄数组是否为空
 * 2. 遍历所有能力句柄
 * 3. 验证句柄的有效性
 * 4. 清除对应的能力
 * 5. 清空句柄数组
 * 
 * @note 函数执行完成后会清空传入的句柄数组
 */
void UWarriorAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	// 检查要移除的能力句柄数组是否为空，为空则直接返回
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}

	// 遍历所有要移除的能力规格句柄
	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		// 验证当前能力句柄是否有效，有效则清除该能力
		if (SpecHandle.IsValid())
		{
			// 从能力系统组件中清除指定的能力
			ClearAbility(SpecHandle);
		}
	}
 
	// 清空句柄数组，释放内存
	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);

		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
		
	}
	
	return false;

	
}
