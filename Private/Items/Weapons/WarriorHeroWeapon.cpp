// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapons/WarriorHeroWeapon.h"

/**
 * @brief 设置当前武器授予的技能能力句柄数组实现
 * 
 * 将传入的能力规范句柄数组赋值给内部存储变量
 * 用于建立武器与相关能力之间的关联
 * 
 * @param InSpecHandles 需要绑定的能力规范句柄数组
 * 
 * @details
 * 1. 直接将传入的句柄数组赋值给GrantedAbilitySpecHandles成员变量
 * 2. 覆盖之前存储的任何句柄数据
 * 3. 不验证句柄的有效性，由调用者确保数据正确性
 */
void AWarriorHeroWeapon::AssignGrantedAbilitySpecHandle(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
	// 将传入的能力规范句柄数组赋值给内部存储变量
	GrantedAbilitySpecHandles = InSpecHandles;
}

/**
 * @brief 获取当前武器已授予的技能能力句柄数组实现
 * 
 * 返回当前武器绑定的能力规范句柄数组的副本
 * 用于在武器使用时激活对应技能能力
 * 
 * @return TArray<FGameplayAbilitySpecHandle> 返回该武器绑定的能力规范句柄数组
 * 
 * @details
 * 1. 返回GrantedAbilitySpecHandles成员变量的副本
 * 2. 函数标记为const，保证不修改对象状态
 * 3. 返回的数组可用于激活与武器关联的能力
 */
TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
	// 返回内部存储的能力规范句柄数组
	return GrantedAbilitySpecHandles;
}