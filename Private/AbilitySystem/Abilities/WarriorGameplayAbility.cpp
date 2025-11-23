// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"

/**
 * @brief 当能力被授予给角色时调用的函数
 * 
 * 该函数在能力被添加到AbilitySystemComponent时自动调用
 * 根据能力激活策略决定是否立即激活能力
 * 
 * @param ActorInfo 包含关于拥有此能力的Actor的信息，如AbilitySystemComponent、AvatarActor等
 * @param Spec 能力规格，包含关于此能力实例的信息，如Handle、Level、SourceObject等
 * 
 * @details
 * 1. 首先调用父类的OnGiveAbility函数确保基础功能正常执行
 * 2. 检查能力激活策略是否为"OnGiven"（即在被授予时立即激活）
 * 3. 确保ActorInfo存在且能力当前未激活
 * 4. 尝试激活该能力
 */
void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilitySpec& Spec)
{
	// 调用父类的OnGiveAbility函数，确保基础功能正常执行
	Super::OnGiveAbility(ActorInfo, Spec);

	// 检查能力激活策略是否为"OnGiven"（即在被授予时立即激活）
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		// 确保ActorInfo存在且能力当前未激活
		if (ActorInfo && !Spec.IsActive())
		{
			// 尝试激活该能力
			// TryActivateAbility会检查能力是否可以激活，并在可以激活时执行激活逻辑
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

/**
 * @brief 当能力结束时调用的函数
 * 
 * 该函数在能力完成或被取消时自动调用
 * 根据能力激活策略决定是否清除能力
 * 
 * @param Handle 能力句柄，用于标识特定的能力实例
 * @param ActorInfo 包含关于拥有此能力的Actor的信息
 * @param ActivationInfo 包含关于此能力激活的信息
 * @param bReplicateEndAbility 是否需要复制能力结束事件到网络上的其他客户端
 * @param bWasCancelled 能力是否被取消而不是正常结束
 * 
 * @details
 * 1. 首先调用父类的EndAbility函数确保基础功能正常执行
 * 2. 检查能力激活策略是否为"OnGiven"
 * 3. 确保ActorInfo存在
 * 4. 清除该能力，将其从能力系统中移除
 */
void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// 调用父类的EndAbility函数，确保基础功能正常执行
	// 这会处理能力结束的基本逻辑，如清理激活信息等
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	// 检查能力激活策略是否为"OnGiven"
	// 对于OnGiven策略的能力，我们需要在结束时将其从ASC中清除
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		// 确保ActorInfo存在
		// ActorInfo包含了能力系统组件等关键信息
		if (ActorInfo)
		{
			// 清除该能力，将其从能力系统中移除
			// ClearAbility会从ASC的能力列表中移除该能力实例
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

/**
 * @brief 从ActorInfo中获取PawnCombatComponent组件的辅助函数
 * 
 * 提供了一个便捷的方法来访问与当前能力关联的Pawn的战斗组件
 * 
 * @return UPawnCombatComponent指针，如果不存在则返回nullptr
 * 
 * @details
 * 1. 获取角色信息中的Avatar Actor（通常是角色本身）
 * 2. 在Avatar Actor中查找PawnCombatComponent组件
 * 3. 返回找到的组件指针，如果未找到则返回nullptr
 */
UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	// 获取角色信息中的Avatar Actor（通常是角色本身），并查找PawnCombatComponent组件
	// FindComponentByClass会在Actor的所有组件中搜索指定类型的组件
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

/**
 * @brief 从ActorInfo中获取WarriorAbilitySystemComponent组件的辅助函数
 * 
 * 提供了一个便捷的方法来访问Warrior特定的能力系统组件
 * 
 * @return UWarriorAbilitySystemComponent指针，如果不存在或类型不匹配则返回nullptr
 * 
 * @details
 * 1. 获取当前ActorInfo中的AbilitySystemComponent
 * 2. 将其转换为UWarriorAbilitySystemComponent类型
 * 3. 返回转换后的指针，如果转换失败则返回nullptr
 */
UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
	// 获取当前ActorInfo中的AbilitySystemComponent，并将其转换为UWarriorAbilitySystemComponent类型
	// Cast是Unreal Engine中的安全类型转换函数，如果转换失败会返回nullptr
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

/**
 * @brief 将游戏效果规格应用到目标Actor的原生实现
 * 
 * 将指定的游戏效果应用到目标Actor，这是应用游戏效果的核心实现函数
 * 包含了必要的有效性检查以避免运行时崩溃
 * 
 * @param TargetActor 目标角色，效果将应用到该角色上
 * @param InSpecHandle 游戏效果规格句柄，包含要应用的效果信息
 * @return FActiveGameplayEffectHandle 活动的游戏效果句柄，可用于后续操作如移除效果
 * 
 * @details
 * 1. 获取目标角色的AbilitySystemComponent
 * 2. 验证目标ASC和效果规格的有效性
 * 3. 应用效果到目标ASC
 * 4. 返回活动效果句柄
 * 
 * @note 使用条件检查替代断言检查，避免游戏因无效参数而崩溃
 * @see BP_ApplyEffectSpecHandleToTarget 蓝图可调用版本
 */
FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	
	// 获取目标角色的能力系统组件
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	// 检查能力和效果规格的有效性
	// 将断言检查改为条件检查，避免游戏崩溃
	if (!TargetASC || !InSpecHandle.IsValid())
	{
		// 返回无效的效果句柄
		return FActiveGameplayEffectHandle();
	}
	
	// 将效果规格应用到目标的能力系统组件
	// 使用当前能力的ASC将效果应用到目标ASC
	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
}

/**
 * @brief 将游戏效果规格应用到目标Actor的蓝图可调用版本
 * 
 * 蓝图可调用函数，封装了NativeApplyEffectSpecHandleToTarget并提供执行结果反馈
 * 便于在蓝图中使用并获取操作结果
 * 
 * @param TargetActor 目标角色，效果将应用到该角色上
 * @param InSpecHandle 游戏效果规格句柄，包含要应用的效果信息
 * @param OutSuccessType 输出参数，指示操作是否成功，可用于蓝图中的执行引脚
 * @return FActiveGameplayEffectHandle 活跃的游戏效果句柄，可用于后续操作如移除效果
 * 
 * @details
 * 1. 调用NativeApplyEffectSpecHandleToTarget执行实际应用逻辑
 * 2. 根据返回的句柄判断操作是否成功
 * 3. 设置输出参数以在蓝图中提供反馈
 * 4. 返回活动效果句柄
 * 
 * @note 使用ExpandEnumAsExecs元数据使SuccessType枚举在蓝图中显示为执行引脚
 * @see NativeApplyEffectSpecHandleToTarget 原生C++实现版本
 */
FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
{
	// 调用原生实现应用效果到目标
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);

	// 根据句柄是否成功应用设置输出类型
	// 如果句柄有效表示应用成功，否则表示应用失败
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;

	// 返回活动的游戏效果句柄
	return ActiveGameplayEffectHandle;
}

void UWarriorGameplayAbility::ApplyGameplayEffectSpecHandleToHitResult(const FGameplayEffectSpecHandle& InSpecHandle,
	const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty())
	{
		return;
	}

	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

	for (const FHitResult& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			if (UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;
					
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitPawn,
						WarriorGameplayTags::Shared_Event_HitReact, Data);
				}
			}
		}
	}
}
