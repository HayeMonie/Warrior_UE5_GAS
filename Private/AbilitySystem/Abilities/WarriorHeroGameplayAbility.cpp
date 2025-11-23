// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"
#include "WarriorGameplayTags.h"

/**
 * @brief 获取当前能力关联的英雄角色实例
 * 
 * 该函数通过缓存机制避免重复的Cast操作，提高性能
 * 如果缓存无效，则从CurrentActorInfo->AvatarActor转换并缓存
 * 
 * @return AWarriorHeroCharacter指针，如果无法获取或转换失败则返回nullptr
 * 
 * @details
 * 1. 首先检查缓存的英雄角色是否有效，避免重复获取和转换
 * 2. 如果缓存无效，则从当前ActorInfo的AvatarActor中获取并转换为AWarriorHeroCharacter
 * 3. AvatarActor通常是实际的游戏角色对象
 * 4. 返回缓存的英雄角色实例，如果无效则返回nullptr
 */
AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	// 检查缓存的英雄角色是否有效，避免重复获取和转换
	// IsValid()检查弱指针是否仍然指向一个有效的对象
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		// 从当前ActorInfo的AvatarActor中获取并转换为AWarriorHeroCharacter
		// AvatarActor通常是实际的游戏角色对象
		// Cast是Unreal Engine的安全类型转换函数，如果转换失败会返回nullptr
		CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	
	// 返回缓存的英雄角色实例，如果无效则返回nullptr
	// IsValid()检查弱指针是否仍然指向一个有效的对象
	// Get()获取弱指针指向的实际对象
	return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
}

/**
 * @brief 获取当前能力关联的英雄控制器实例
 * 
 * 该函数通过缓存机制避免重复的Cast操作，提高性能
 * 如果缓存无效，则从CurrentActorInfo->PlayerController转换并缓存
 * 
 * @return AWarriorHeroController指针，如果无法获取或转换失败则返回nullptr
 * 
 * @details
 * 1. 首先检查缓存的英雄控制器是否有效，避免重复获取和转换
 * 2. 如果缓存无效，则从当前ActorInfo的PlayerController中获取并转换为AWarriorHeroController
 * 3. PlayerController负责处理玩家输入和控制角色行为
 * 4. 返回缓存的英雄控制器实例，如果无效则返回nullptr
 */
AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	// 检查缓存的英雄控制器是否有效，避免重复获取和转换
	// IsValid()检查弱指针是否仍然指向一个有效的对象
	if (!CachedWarriorHeroController.IsValid())
	{
		// 从当前ActorInfo的PlayerController中获取并转换为AWarriorHeroController
		// PlayerController负责处理玩家输入和控制角色行为
		// Cast是Unreal Engine的安全类型转换函数，如果转换失败会返回nullptr
		CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}

	// 返回缓存的英雄控制器实例，如果无效则返回nullptr
	// IsValid()检查弱指针是否仍然指向一个有效的对象
	// Get()获取弱指针指向的实际对象
	return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
}

/**
 * @brief 获取当前英雄角色的战斗组件
 * 
 * 该函数是获取英雄角色战斗相关功能的便捷方法
 * 通过调用GetHeroCharacterFromActorInfo获取英雄角色，然后获取其HeroCombatComponent组件
 * 
 * @return UHeroCombatComponent指针，用于访问英雄的战斗功能
 * 
 * @details
 * 1. 通过调用GetHeroCharacterFromActorInfo获取英雄角色
 * 2. 调用英雄角色的GetHeroCombatComponent方法获取战斗组件
 * 3. HeroCombatComponent包含了英雄角色的战斗相关逻辑和数据
 */
UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	// 通过调用GetHeroCharacterFromActorInfo获取英雄角色，然后获取其HeroCombatComponent组件
	// HeroCombatComponent包含了英雄角色的战斗相关逻辑和数据
	// GetHeroCombatComponent()是AWarriorHeroCharacter类的方法，返回UHeroCombatComponent组件
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UHeroUIComponent* UWarriorHeroGameplayAbility::GetHeroUIComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroUIComponent();
}

/**
 * 创建英雄伤害效果的规格句柄
 * 
 * @param EffectClass 伤害效果的类
 * @param InWeaponBaseDamage 武器基础伤害值
 * @param InCurrentAttackTypeTag 当前攻击类型标签
 * @param InUsedComboCount 已使用的连击次数
 * @return 包含伤害效果规格的句柄
 */
FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);
	
	// 创建效果上下文句柄，用于存储效果的相关信息
	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	
	// 设置效果的来源能力
	ContextHandle.SetAbility(this);
	
	// 添加源对象（通常是角色的Avatar Actor）
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	
	// 添加引发效果的角色和目标（这里都设置为Avatar Actor）
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	// 创建传出效果的规格句柄
	FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo() -> MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);

	// 设置由调用者指定的基础伤害值
	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	// 如果攻击类型标签有效，则设置该标签对应的连击次数
	if (InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}

	return EffectSpecHandle;
}

bool UWarriorHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldown,
	float& RemainingCooldown)
{
	check(InCooldownTag.IsValid());

	FGameplayEffectQuery CooldownQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());
	
	TArray< TPair<float, float> > TimeRemainingAndDuration = GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);

	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldown = TimeRemainingAndDuration[0].Key;
		TotalCooldown = TimeRemainingAndDuration[0].Value;
	}

	return RemainingCooldown > 0.f;
}

