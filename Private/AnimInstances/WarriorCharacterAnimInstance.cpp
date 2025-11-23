// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "Characters/WarriorBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * @brief 原生初始化动画函数实现
 * 
 * 在动画蓝图初始化时调用，用于初始化动画相关的变量和引用
 * 重写父类函数以添加角色特有的初始化逻辑
 * 
 * @details
 * 1. 通过TryGetPawnOwner获取拥有此动画实例的Pawn角色对象
 * 2. 将获取到的Pawn对象转换为AWarriorBaseCharacter类型
 * 3. 如果转换成功，获取角色的CharacterMovementComponent组件引用
 * 
 * @note 该函数在动画实例创建时调用一次
 */
void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	// 尝试获取拥有此动画实例的Pawn对象，并转换为AWarriorBaseCharacter类型
	// TryGetPawnOwner是UAnimInstance提供的函数，用于获取拥有此动画实例的Pawn
	OwningCharacter = Cast<AWarriorBaseCharacter>(TryGetPawnOwner());
	
	// 检查角色对象是否有效
	if (OwningCharacter)
	{
		// 获取角色的移动组件引用，用于后续的移动相关数据获取
		// GetCharacterMovement是ACharacter提供的函数，返回CharacterMovementComponent
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

/**
 * @brief 原生线程安全更新动画函数实现
 * 
 * 在每帧更新时调用，用于更新动画相关的变量
 * 该函数在线程安全的环境下执行
 * 重写父类函数以添加角色特有的更新逻辑
 * 
 * @param DeltaSeconds 距离上次更新经过的时间（秒）
 * 
 * @details
 * 1. 检查角色和移动组件引用的有效性
 * 2. 计算角色在地面上的移动速度
 * 3. 检查角色是否具有加速度（是否在主动移动）
 * 
 * @note 该函数每帧调用，应避免过于复杂的计算
 */
void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	// 检查角色和移动组件引用是否有效，无效则直接返回
	// 避免访问空指针导致崩溃
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}

	// 计算角色在地面上的移动速度（2D平面上的速度大小）
	// GetVelocity()返回角色的三维速度向量
	// Size2D()计算二维平面上的速度大小（忽略Z轴）
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();

	// 检查角色是否具有加速度（是否在主动移动）
	// GetCurrentAcceleration()获取当前加速度向量
	// SizeSquared2D()计算二维平面上加速度大小的平方
	// 与0比较平方值避免开方运算，提高性能
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());



	
}