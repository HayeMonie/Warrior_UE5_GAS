// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"

/**
 * @brief 原生初始化动画函数实现
 * 
 * 在动画蓝图初始化时调用，用于初始化动画相关的变量和引用
 * 重写父类函数以添加英雄角色特有的初始化逻辑
 * 
 * @details
 * 1. 调用父类的NativeInitializeAnimation函数执行基础初始化
 * 2. 检查OwningCharacter是否有效
 * 3. 将OwningCharacter转换为AWarriorHeroCharacter并赋值给OwningHeroCharacter
 * 
 * @note 依赖于父类已初始化的OwningCharacter指针
 */
void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	// 调用父类的初始化函数，确保基础动画初始化完成
	Super::NativeInitializeAnimation();

	// 检查拥有的角色对象是否有效
	if (OwningCharacter)
	{
		// 将基础角色对象转换为英雄角色对象并保存引用
		// Cast是Unreal Engine的安全类型转换函数
		OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
	}
}

/**
 * @brief 原生线程安全更新动画函数实现
 * 
 * 在每帧更新时调用，用于更新动画相关的变量
 * 该函数在线程安全的环境下执行
 * 重写父类函数以添加英雄角色特有的更新逻辑
 * 
 * @param DeltaSeconds 距离上次更新经过的时间（秒）
 * 
 * @details
 * 1. 调用父类的更新函数执行基础更新逻辑
 * 2. 检查角色是否有加速度
 * 3. 如果有加速度，则重置空闲时间和休闲状态标志
 * 4. 如果无加速度，则累计空闲时间并检查是否达到休闲状态阈值
 * 
 * @note 该函数每帧调用，应避免过于复杂的计算
 */
void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	// 调用父类的更新函数，确保基础动画更新完成
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	// 检查角色是否具有加速度（是否在移动）
	if (bHasAcceleration)
	{
		// 如果角色正在移动，重置空闲时间计数器
		IdleElapsedTime = 0.f;
		
		// 设置不进入休闲状态
		bShouldEnterRelaxState = false;
	}
	else
	{
		// 如果角色未移动，累加空闲时间
		IdleElapsedTime += DeltaSeconds;
		
		// 检查空闲时间是否超过阈值，决定是否进入休闲状态
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThreshold);
	}
}