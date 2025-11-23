// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameFramework/Character.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "MotionWarpingComponent.h"

#include "WarriorBaseCharacter.generated.h"


class UDataAsset_StartUpDataBase;
class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
class UMotionWarpingComponent;

/**
 * @brief 基础角色类
 * 
 * 继承自ACharacter，实现IAbilitySystemInterface和IPawnCombatInterface接口
 * 作为所有角色（包括英雄和敌人）的基类，提供通用的角色功能
 * 
 * @details
 * 1. 集成Ability System系统，提供能力管理和属性系统支持
 * 2. 实现战斗接口，支持战斗相关功能
 * 3. 提供角色初始化和管理功能
 * 
 * @see ACharacter
 * @see IAbilitySystemInterface
 * @see IPawnCombatInterface
 */
UCLASS()
class WARRIOR_API AWarriorBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief 构造函数
	 * 
	 * 设置角色的默认属性值
	 * 初始化能力系统组件和属性集组件
	 * 
	 * @details
	 * 1. 禁用角色的Tick功能以提高性能
	 * 2. 创建并初始化能力系统组件
	 * 3. 创建并初始化属性集组件
	 */
	AWarriorBaseCharacter();

	//~ Begin IAbilitySystemInterface Interface.
	/**
	 * @brief 获取能力系统组件
	 * 
	 * 实现IAbilitySystemInterface接口，返回角色的能力系统组件
	 * 
	 * @return 返回角色的能力系统组件指针
	 * 
	 * @see UAbilitySystemComponent
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface Interface.

	//~ Begin IPawnCombatInterface Interface.
	/**
	 * @brief 获取角色战斗组件
	 * 
	 * 实现IPawnCombatInterface接口，返回角色的战斗组件
	 * 
	 * @return 返回角色的战斗组件指针，当前实现返回nullptr
	 * 
	 * @see UPawnCombatComponent
	 */
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface.

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//~ End IPawnUIInterface Interface.


	
protected:
	//~ Begin APawn Interface.
	/**
	 * @brief 角色被控制器占有时的回调函数
	 * 
	 * 当角色被AI控制器或玩家控制器占有时调用
	 * 用于初始化能力系统和应用启动数据
	 * 
	 * @param NewController 占有角色的新控制器
	 * 
	 * @details
	 * 1. 调用父类的PossessedBy函数
	 * 2. 初始化能力系统组件的Actor信息
	 * 3. 将属性集注册到能力系统组件
	 * 4. 检查并确保启动数据已分配
	 */
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface.
	
	/**
	 * @brief 战士能力系统组件
	 * 
	 * 角色的能力系统组件，负责管理角色的所有能力
	 * 处理能力的授予、激活、冷却等逻辑
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	/**
	 * @brief 战士属性集组件
	 * 
	 * 角色的属性集组件，包含角色的所有属性
	 * 如生命值、怒气值、攻击力、防御力等
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	/**
	 * @brief 角色启动数据
	 * 
	 * 角色的启动数据资产，包含角色初始化时需要的数据
	 * 使用软引用避免加载时的循环依赖问题
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	/**
	 * @brief 获取战士能力系统组件
	 * 
	 * 内联函数，用于快速获取战士能力系统组件
	 * 
	 * @return 返回战士能力系统组件指针
	 */
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const
	{
		return WarriorAbilitySystemComponent;
	}

	/**
	 * @brief 获取战士属性集组件
	 * 
	 * 内联函数，用于快速获取战士属性集组件
	 * 
	 * @return 返回战士属性集组件指针
	 */
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const
	{
		return WarriorAttributeSet;
	}
};