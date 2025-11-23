// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorHeroAnimInstance.generated.h"

/**
 * @brief 英雄角色动画实例类
 * 
 * 继承自UWarriorCharacterAnimInstance，专门用于英雄角色的动画逻辑处理
 * 负责处理英雄角色特有的动画状态，如休闲状态等
 * 
 * @details
 * 1. 管理英雄角色的动画蓝图逻辑
 * 2. 处理角色进入休闲状态的逻辑
 * 3. 继承并扩展基础角色动画实例的功能
 * 
 * @see UWarriorCharacterAnimInstance
 */
UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()

public:
	/**
	 * @brief 原生初始化动画函数
	 * 
	 * 在动画蓝图初始化时调用，用于初始化动画相关的变量和引用
	 * 重写父类函数以添加英雄角色特有的初始化逻辑
	 * 
	 * @details
	 * 1. 调用父类的初始化函数
	 * 2. 获取并设置英雄角色的引用
	 */
	virtual void NativeInitializeAnimation() override;
	
	/**
	 * @brief 原生线程安全更新动画函数
	 * 
	 * 在每帧更新时调用，用于更新动画相关的变量
	 * 该函数在线程安全的环境下执行
	 * 重写父类函数以添加英雄角色特有的更新逻辑
	 * 
	 * @param DeltaSeconds 距离上次更新经过的时间（秒）
	 * 
	 * @details
	 * 1. 调用父类的更新函数
	 * 2. 更新角色休闲状态的判断逻辑
	 */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	/**
	 * @brief 拥有的英雄角色引用
	 * 
	 * 指向当前动画实例所属的英雄角色对象
	 * 用于访问角色特有的属性和方法
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	AWarriorHeroCharacter* OwningHeroCharacter;

	/**
	 * @brief 是否应该进入休闲状态
	 * 
	 * 标识角色是否应该进入休闲动画状态的布尔值
	 * 当角色长时间静止不动时设置为true
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	/**
	 * @brief 进入休闲状态的阈值时间
	 * 
	 * 角色保持静止多长时间后进入休闲状态的阈值（秒）
	 * 可在编辑器中调整该值以改变进入休闲状态的时间
	 */
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.f;

	/**
	 * @brief 空闲经过的时间
	 * 
	 * 角色保持无加速度状态（静止）的累计时间（秒）
	 * 用于判断是否达到进入休闲状态的阈值
	 */
	float IdleElapsedTime;     // Idling多少时间
};
