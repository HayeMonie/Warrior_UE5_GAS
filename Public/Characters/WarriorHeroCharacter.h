// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "WarriorHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
class UHeroCombatComponent;

/**
 * @brief 英雄角色类
 * 
 * 继承自AWarriorBaseCharacter，代表游戏中的玩家控制角色（英雄）
 * 实现了第三人称视角控制、输入处理、摄像机系统等功能
 * 
 * @details
 * 1. 集成摄像机系统，支持第三人称视角
 * 2. 实现增强输入系统，处理玩家输入
 * 3. 集成英雄战斗组件，处理战斗相关逻辑
 * 4. 支持能力系统输入处理
 * 
 * @see AWarriorBaseCharacter
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()


public:
	/**
	 * @brief 构造函数
	 * 
	 * 设置英雄角色的默认属性值
	 * 初始化摄像机组件、战斗组件等
	 */
	AWarriorHeroCharacter();


	//~ Begin IPawnCombatInterface Interface.
	/**
	 * @brief 获取角色战斗组件
	 * 
	 * 实现IPawnCombatInterface接口，返回英雄的战斗组件
	 * 
	 * @return 返回英雄的战斗组件指针
	 * 
	 * @see UHeroCombatComponent
	 */
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	//~ End IPawnCombatInterface Interface.

	//~ Begin IPawnUIInterface Interface.
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UHeroUIComponent* GetHeroUIComponent() const override;
	//~ End IPawnUIInterface Interface.
	
protected:

	//~ Begin APawn Interface.
	/**
	 * @brief 角色被控制器占有时的回调函数
	 * 
	 * 当角色被玩家控制器占有时调用
	 * 用于初始化能力系统和应用启动数据
	 * 
	 * @param NewController 占有角色的新控制器（玩家控制器）
	 * 
	 * @details
	 * 1. 调用父类的PossessedBy函数
	 * 2. 同步加载并应用启动数据到能力系统组件
	 */
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface.
	
	/**
	 * @brief 设置玩家输入组件
	 * 
	 * 重写父类函数，用于绑定玩家输入动作
	 * 配置增强输入系统和输入映射
	 * 
	 * @param PlayerInputComponent 玩家输入组件指针
	 * 
	 * @details
	 * 1. 验证输入配置数据资产的有效性
	 * 2. 获取并配置增强输入子系统
	 * 3. 绑定移动、视角和能力输入动作
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/**
	 * @brief 游戏开始时调用
	 * 
	 * 重写父类函数，在游戏开始时执行初始化逻辑
	 * 确保角色的旋转和移动设置正确
	 */
	virtual void BeginPlay() override;


	
private:
	
#pragma region components
	/**
	 * @brief 摄像机摇臂组件
	 * 
	 * 用于实现第三人称视角的摄像机摇臂
	 * 控制摄像机与角色的距离和角度
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/**
	 * @brief 跟随摄像机组件
	 * 
	 * 第三人称视角的主摄像机
	 * 附加在摄像机摇臂上，跟随角色移动
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/**
	 * @brief 英雄战斗组件
	 * 
	 * 英雄特有的战斗组件，处理英雄战斗相关逻辑
	 * 如武器管理、技能释放等
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHeroUIComponent* HeroUIComponent;

	
#pragma endregion


#pragma region Inputs
	
	/**
	 * @brief 输入配置数据资产
	 * 
	 * 包含输入映射配置的数据资产
	 * 定义了输入动作与游戏行为的映射关系
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;

	UPROPERTY()
	FVector2D SwitchDirection = FVector2D::ZeroVector;

	/**
	 * @brief 处理角色移动输入
	 * 
	 * 处理玩家的移动输入，控制角色在世界中的移动
	 * 
	 * @param InputActionValue 输入动作的值，包含移动方向信息
	 */
	void Input_Move(const FInputActionValue& InputActionValue);
	
	/**
	 * @brief 处理视角控制输入
	 * 
	 * 处理玩家的视角控制输入，控制摄像机的视角方向
	 * 
	 * @param InputActionValue 输入动作的值，包含视角控制方向信息
	 */
	void Input_Look(const FInputActionValue& InputActionValue);

	void Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue);
	void Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue);

	void Input_PickUpStonesStarted(const FInputActionValue& InputActionValue);

	/**
	 * @brief 处理能力输入按下事件
	 * 
	 * 处理玩家按下能力相关输入时的逻辑
	 * 通知能力系统组件处理输入按下事件
	 * 
	 * @param InInputTag 与按下输入相关联的GameplayTag
	 */
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	
	/**
	 * @brief 处理能力输入释放事件
	 * 
	 * 处理玩家释放能力相关输入时的逻辑
	 * 通知能力系统组件处理输入释放事件
	 * 
	 * @param InInputTag 与释放输入相关联的GameplayTag
	 */
	void Input_AbilityInputReleased(FGameplayTag InInputTag);
	
	
#pragma endregion

public:
	/**
	 * @brief 获取英雄战斗组件
	 * 
	 * 内联函数，用于快速获取英雄战斗组件
	 * 
	 * @return 返回英雄战斗组件指针
	 */
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const
	{
		return HeroCombatComponent;
	}
	
	
};