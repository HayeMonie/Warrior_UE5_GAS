// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"
#include "Components/BoxComponent.h"
#include "WarriorFunctionLibrary.h"
#include "Items/Weapons/WarriorWeaponBase.h"

#include "WarriorDebugHelper.h"

/**
 * @brief 注册已生成的武器到角色武器映射中实现
 * 
 * 将新生成的武器实例与Gameplay标签关联并存储在武器映射中
 * 可选择性地将该武器设置为当前装备的武器
 * 
 * @param InWeaponTagToRegister 用于标识武器的Gameplay标签
 * @param InWeaponToRegister 需要注册的武器实例指针
 * @param bRegisterAsEquippedWeapon 是否将该武器注册为当前装备的武器，默认为false
 * 
 * @details
 * 1. 检查武器标签是否已存在，避免重复注册
 * 2. 验证武器实例指针的有效性
 * 3. 将武器添加到角色携带的武器映射表中
 * 4. 绑定武器的事件处理函数
 * 5. 根据需要设置为当前装备的武器
 */
void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
                                                 AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	// 检查武器标签是否已存在，如果存在则触发断言并输出错误信息
	// checkf是带格式化字符串的断言，用于验证条件并提供详细错误信息
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A name named %s has been already added as a carried weapon"), *InWeaponTagToRegister.ToString());
	
	// 检查武器实例指针的有效性
	check(InWeaponToRegister);

	// 将武器标签和武器实例添加到角色携带的武器映射表中
	// Emplace是TMap的高效添加方法，避免不必要的拷贝
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	// 绑定武器的击中目标事件到本组件的OnHitTargetActor处理函数
	// BindUObject用于将 UObject 的成员函数绑定到委托
	InWeaponToRegister -> OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	
	// 绑定武器的从目标拔出事件到本组件的OnWeaponPulledFromTarget处理函数
	InWeaponToRegister -> OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTarget);
	
	// 如果需要将该武器注册为当前装备的武器
	if (bRegisterAsEquippedWeapon)
	{
		// 设置当前装备武器标签为新注册的武器标签
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
}

/**
 * @brief 根据Gameplay标签获取角色携带的武器实现
 * 
 * 在角色携带的武器映射表中查找指定标签对应的武器实例
 * 
 * @param InWeaponTagToGet 用于查找武器的Gameplay标签
 * @return 返回找到的武器实例指针，如果未找到则返回nullptr
 * 
 * @details
 * 1. 使用TMap::Find方法在武器映射表中查找
 * 2. 如果找到则返回武器实例指针，否则返回nullptr
 */
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	// 使用TMap::Find方法在武器映射表中查找指定标签对应的武器
	// Find返回指向值的指针，如果未找到则返回nullptr
	// 使用结构化绑定声明获取查找结果
	if (AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
	{
		// 找到武器，返回武器实例指针
		return *FoundWeapon; // Valid weapon
	}
	
	// 未找到对应标签的武器，返回空指针
	return nullptr; // Not found
}

/**
 * @brief 获取角色当前装备的武器实例实现
 * 
 * 根据当前装备武器标签获取对应的武器实例
 * 
 * @return 返回当前装备的武器实例指针，如果未装备则返回nullptr
 * 
 * @details
 * 1. 验证当前装备武器标签的有效性
 * 2. 调用GetCharacterCarriedWeaponByTag获取武器实例
 */
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeaponTag() const
{
	// 检查当前装备武器标签是否有效
	// IsValid是FGameplayTag的方法，用于检查标签是否有效
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		// 标签无效，返回空指针
		return nullptr;
	}

	// 根据当前装备武器标签获取对应的武器实例
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

/**
 * @brief 切换武器碰撞检测的启用状态实现
 * 
 * 启用或禁用指定武器的碰撞检测功能
 * 
 * @param bShouldEnable 是否启用武器碰撞检测
 * @param ToggleDamageType 指定要切换碰撞检测的武器或身体部位类型，默认为当前装备武器
 * 
 * @details
 * 1. 根据ToggleDamageType类型确定要操作的武器
 * 2. 设置武器碰撞盒的启用状态
 * 3. 在禁用时清空重叠演员列表
 */
void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	// 检查是否要切换当前装备武器的碰撞检测
	if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
	{
		ToggleCurrentEquippedWeaponCollision(bShouldEnable);
	}
	// Handle body collision boxes
	// 处理身体碰撞盒逻辑
	else
	{
		ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);
	}

	
}

/**
 * @brief 武器击中目标演员时的回调函数实现
 * 
 * 当武器击中目标时调用，用于处理击中逻辑
 * 
 * @param HitActor 被击中的目标演员
 * 
 * @details
 * 该函数为虚函数，可在派生类中重写以实现具体逻辑
 * 当前实现为空，待后续完善
 */
void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// TODO: 实现击中目标的具体逻辑
	// 可能包括：
	// 1. 应用伤害效果
	// 2. 播放击中特效
	// 3. 触发音效
	// 4. 更新UI信息等
}

/**
 * @brief 武器从目标中拔出时的回调函数实现
 * 
 * 当武器从目标中拔出时调用，用于处理拔出逻辑
 * 
 * @param InteractedActor 交互的目标演员
 * 
 * @details
 * 该函数为虚函数，可在派生类中重写以实现具体逻辑
 * 当前实现为空，待后续完善
 */
void UPawnCombatComponent::OnWeaponPulledFromTarget(AActor* InteractedActor)
{
	// TODO: 实现武器拔出的具体逻辑
	// 可能包括：
	// 1. 处理拔出时的伤害或效果
	// 2. 播放拔出特效
	// 3. 触发音效
	// 4. 更新相关状态等

	// if (APawn* HitPawn = Cast<APawn>(OtherActor))
	// {
	// 	if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
	// 	{
	// 		OnWeaponHitTarget.ExecuteIfBound(OtherActor);
	// 	}
	// }
}

void UPawnCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable)
{
	// 获取当前装备的武器实例
	AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeaponTag();

	// 检查武器实例的有效性
	check(WeaponToToggle);
		
	// 根据启用标志设置武器碰撞盒的启用状态
	if (bShouldEnable)
	{
		// 启用碰撞检测，设置为QueryOnly模式（仅查询，不产生物理反应）
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		// 禁用碰撞检测
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 清空重叠演员列表，为下一次启用碰撞做准备
		OverlappedActors.Empty();
	}
}

void UPawnCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	
}
