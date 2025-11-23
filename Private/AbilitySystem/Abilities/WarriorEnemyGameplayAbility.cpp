/**
 * @file WarriorEnemyGameplayAbility.cpp
 * @brief 敌人游戏能力实现文件
 * 
 * 实现了敌人角色专属的游戏能力类，提供获取敌人角色和战斗组件的功能
 */

#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "WarriorGameplayTags.h"

/**
 * @brief 获取并缓存当前技能相关的敌人角色对象指针
 * 
 * 该函数通过缓存机制避免重复的Cast操作，提高性能
 * 如果缓存无效，则从CurrentActorInfo->AvatarActor转换并缓存
 * 
 * @return AWarriorEnemyCharacter指针，如果无法获取或转换失败则返回nullptr
 * 
 * @details
 * 1. 首先检查缓存的敌人角色是否有效，避免重复获取和转换
 * 2. 如果缓存无效，则从当前ActorInfo的AvatarActor中获取并转换为AWarriorEnemyCharacter
 * 3. AvatarActor通常是实际的游戏角色对象
 * 4. 返回缓存的敌人角色实例，如果无效则返回nullptr
 */
AWarriorEnemyCharacter* UWarriorEnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	// 检查缓存的敌人角色是否有效，避免重复获取和转换
	// IsValid()检查弱指针是否仍然指向一个有效的对象
	if (!CachedWarriorEnemyCharacter.IsValid())
	{
		// 从当前ActorInfo的AvatarActor中获取并转换为AWarriorEnemyCharacter
		// AvatarActor通常是实际的游戏角色对象
		// Cast是Unreal Engine的安全类型转换函数，如果转换失败会返回nullptr
		// CurrentActorInfo->AvatarActor是拥有此能力的角色对象
		CachedWarriorEnemyCharacter = Cast<AWarriorEnemyCharacter>(CurrentActorInfo->AvatarActor);
	}

	// 返回缓存的敌人角色实例，如果无效则返回nullptr
	// IsValid()检查弱指针是否仍然指向一个有效的对象
	// Get()获取弱指针指向的实际对象
	return CachedWarriorEnemyCharacter.IsValid() ? CachedWarriorEnemyCharacter.Get() : nullptr;
}

/**
 * @brief 基于敌人角色获取其战斗组件指针
 * 
 * 该函数是获取敌人角色战斗相关功能的便捷方法
 * 通过调用GetEnemyCharacterFromActorInfo获取敌人角色，然后获取其EnemyCombatComponent组件
 * 
 * @return UEnemyCombatComponent指针，用于访问敌人的战斗功能
 * 
 * @details
 * 1. 通过调用GetEnemyCharacterFromActorInfo获取敌人角色
 * 2. 调用敌人角色的GetEnemyCombatComponent方法获取战斗组件
 * 3. EnemyCombatComponent包含了敌人角色的战斗相关逻辑和数据
 */
UEnemyCombatComponent* UWarriorEnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	// 基于上一个函数，直接获取敌人角色的战斗组件指针（UEnemyCombatComponent*）
	// 方便技能系统访问和操作敌人的战斗功能
	// GetEnemyCombatComponent()是AWarriorEnemyCharacter类的方法，返回UEnemyCombatComponent组件
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}

FGameplayEffectSpecHandle UWarriorEnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
	check(EffectClass);

	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	
	// 设置效果的来源能力
	ContextHandle.SetAbility(this);
	
	// 添加源对象（通常是角色的Avatar Actor）
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	
	// 添加引发效果的角色和目标（这里都设置为Avatar Actor）
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo() -> MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
		InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel()));

	return EffectSpecHandle;
	
	
}
