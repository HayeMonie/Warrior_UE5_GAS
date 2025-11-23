// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

/**
 * WarriorAbilitySystemComponent类
 * 
 * 自定义的能力系统组件，继承自UAbilitySystemComponent
 * 负责处理游戏中的能力系统逻辑，包括能力的授予、输入处理等
 */
UCLASS(BlueprintType, Blueprintable)
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	 * 处理能力输入按下的函数
	 * 
	 * @param InInputTag 与按下输入相关联的GameplayTag
	 */
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	
	/**
	 * 处理能力输入释放的函数
	 * 
	 * @param InInputTag 与释放输入相关联的GameplayTag
	 */
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	/**
	 * 授予英雄武器能力的函数
	 * 
	 * @param InDefaultWeaponAbilities 要授予的武器能力集合
	 * @param ApplyLevel 能力应用的等级
	 * @param OutGrantedAbilitySpecHandles 输出参数，返回授予的能力规格句柄数组
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,
		const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel,
		TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);      // 在暴露给蓝图或需要持久化的变量中使用 int32

	/**
	 * 移除已授予的英雄武器能力
	 * 
	 * @param InSpecHandlesToRemove 要移除的能力规格句柄数组的引用
	 * 通过 UPARAM(Ref) 声明InSpecHandlesToRemove以引用方式传递 允许函数修改外部数组
	 */
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemovedGrantedHeroWeaponAbilities(UPARAM(Ref) TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
	
};