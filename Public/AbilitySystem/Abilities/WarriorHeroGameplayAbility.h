// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "Controllers/WarriorHeroController.h"
#include "WarriorHeroGameplayAbility.generated.h"



/**
 * @class AWarriorHeroCharacter
 * @brief 前向声明英雄角色类
 * 
 * 避免头文件循环包含，实际定义在Characters/WarriorHeroCharacter.h中
 */
class AWarriorHeroCharacter;

/**
 * @class AWarriorHeroController
 * @brief 前向声明英雄控制器类
 * 
 * 避免头文件循环包含，实际定义在Controllers/WarriorHeroController.h中
 */
class AWarriorHeroController;

/**
 * @class UWarriorHeroGameplayAbility
 * @brief Warrior英雄角色游戏能力类
 * 
 * 这是一个专门针对英雄角色的游戏能力类，继承自WarriorGameplayAbility
 * 提供了获取英雄角色、英雄控制器和英雄战斗组件的便捷方法
 * 通过缓存机制优化性能，避免重复的对象转换操作
 * 
 * 主要功能:
 * 1. 提供获取英雄角色实例的方法
 * 2. 提供获取英雄控制器实例的方法
 * 3. 提供获取英雄战斗组件的方法
 * 4. 使用缓存机制优化性能
 */
UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()


public:
	/**
	 * @brief 获取当前能力关联的英雄角色实例
	 * 
	 * 该函数通过缓存机制避免重复的Cast操作，提高性能
	 * 如果缓存无效，则从CurrentActorInfo->AvatarActor转换并缓存
	 * 
	 * @return AWarriorHeroCharacter指针，如果无法获取则返回nullptr
	 * 
	 * @note BlueprintPure表示该函数不会改变对象状态，可在蓝图中作为Get节点使用
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

	/**
	 * @brief 获取当前能力关联的英雄控制器实例
	 * 
	 * 该函数通过缓存机制避免重复的Cast操作，提高性能
	 * 如果缓存无效，则从CurrentActorInfo->PlayerController转换并缓存
	 * 
	 * @return AWarriorHeroController指针，如果无法获取则返回nullptr
	 * 
	 * @note BlueprintPure表示该函数不会改变对象状态，可在蓝图中作为Get节点使用
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AWarriorHeroController* GetHeroControllerFromActorInfo();

	/**
	 * @brief 获取当前英雄角色的战斗组件
	 * 
	 * 该函数是获取英雄角色战斗相关功能的便捷方法
	 * 通过调用GetHeroCharacterFromActorInfo获取英雄角色，然后获取其HeroCombatComponent组件
	 * 
	 * @return UHeroCombatComponent指针，用于访问英雄的战斗功能
	 * 
	 * @note BlueprintPure表示该函数不会改变对象状态，可在蓝图中作为Get节点使用
	 */
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UHeroUIComponent* GetHeroUIComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount);
	
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldown, float& RemainingCooldown);


	
private:
	/**
	 * @brief 缓存的英雄角色引用
	 * 
	 * 使用TWeakObjectPtr避免循环引用和内存泄漏
	 * TWeakObjectPtr是一种弱引用，当对象被销毁时会自动置为无效状态
	 * 这样可以避免强引用导致的对象无法被垃圾回收
	 */
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
	
	/**
	 * @brief 缓存的英雄控制器引用
	 * 
	 * 使用TWeakObjectPtr避免循环引用和内存泄漏
	 * TWeakObjectPtr是一种弱引用，当对象被销毁时会自动置为无效状态
	 * 这样可以避免强引用导致的对象无法被垃圾回收
	 */
	TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;
	
};