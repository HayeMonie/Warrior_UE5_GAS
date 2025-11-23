// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Combat/HeroCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Items/Weapons/WarriorHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"

/**
 * 根据武器标签获取英雄携带的武器对象
 * 通过调用父类的GetCharacterCarriedWeaponByTag函数获取武器，然后将其转换为英雄武器类型
 * 
 * @param InWeaponTag 武器标签，用于标识特定的武器类型
 * @return 返回对应标签的AWarriorHeroWeapon对象指针，如果未找到或转换失败则返回nullptr
 */
AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

/**
 * 获取英雄当前装备的武器对象
 * 通过调用父类的GetCharacterCurrentEquippedWeaponTag函数获取当前装备武器的标签，
 * 然后查找对应的武器对象并转换为英雄武器类型
 * 
 * @return 返回当前装备的AWarriorHeroWeapon对象指针，如果没有装备武器或转换失败则返回nullptr
 */
AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeaponTag());
	
}

/**
 * 获取英雄当前装备武器在指定等级下的基础伤害值
 * 首先获取当前装备的武器，然后从武器数据中获取基础伤害的可扩展浮点值，
 * 最后计算指定等级下的实际伤害值
 * 
 * @param InLevel 武器等级，用于计算对应等级的伤害值
 * @return 返回指定等级下的武器基础伤害值
 */
float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
	return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

/**
 * 当武器命中目标时调用的事件处理函数实现
 * 防止重复处理同一目标，创建游戏事件数据并发送给拥有该组件的角色
 * 
 * @param HitActor 被命中的目标Actor
 */
void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	// 检查目标是否已经处理过，避免重复处理
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	// 将目标添加到已处理列表中
	OverlappedActors.AddUnique(HitActor);

	// 创建游戏事件数据
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();  // 设置事件发起者为拥有该组件的Pawn
	Data.Target = HitActor;             // 设置事件目标为被命中的Actor
	
	// 发送近战命中事件给拥有该组件的Pawn
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(),
		WarriorGameplayTags::Shared_Event_MeleeHit, Data);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause, FGameplayEventData());
	




}

/**
 * 当武器从目标上移开时调用的事件处理函数实现
 * 当前为空实现，可在此添加处理逻辑
 * 
 * @param InteractedActor 交互的目标Actor
 */
void UHeroCombatComponent::OnWeaponPulledFromTarget(AActor* InteractedActor)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(),
	WarriorGameplayTags::Player_Event_HitPause, FGameplayEventData());
	
	// TODO: 在此处添加武器从目标上移开时的处理逻辑

}