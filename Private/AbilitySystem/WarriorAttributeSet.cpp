// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

/**
 * @brief UWarriorAttributeSet构造函数
 * 
 * 初始化角色的各种属性值，包括生命值、怒气值、攻击力、防御力等
 * 这些属性构成了角色在游戏中的基础数值系统
 * 
 * @details
 * 1. 初始化当前生命值为1
 * 2. 初始化最大生命值为1
 * 3. 初始化当前怒气值为1
 * 4. 初始化最大怒气值为1
 * 5. 初始化攻击力为1
 * 6. 初始化防御力为1
 * 7. 初始化承受伤害值为0
 * 
 * @note 所有属性名称请勿进行二次修改，否则会引发大量的联动错误
 * @see InitCurrentHealth, InitMaxHealth, InitCurrentRage, InitMaxRage, InitAttackPower, InitDefensePower, InitDamageTaken
 */
UWarriorAttributeSet::UWarriorAttributeSet()
{
	// 初始化当前生命值为1
	InitCurrentHealth(1.f);
	
	// 初始化最大生命值为1
	InitMaxHealth(1.f);
	
	// 初始化当前怒气值为1
	InitCurrentRage(1.f);
	
	// 初始化最大怒气值为1
	InitMaxRage(1.f);
	
	// 初始化攻击力为1
	InitAttackPower(1.f);
	
	// 初始化防御力为1
	InitDefensePower(1.f);
	
	// 初始化承受伤害值为0
	InitDamageTaken(0.f);
}

/**
 * @brief 游戏效果执行后的回调函数
 * 
 * 当GameplayEffect修改了属性值后，该函数会被自动调用
 * 用于处理属性值变更后的逻辑，如数值限制、副作用处理等
 * 
 * @param Data 包含GameplayEffect执行相关信息的回调数据结构
 * 
 * @details
 * 1. 检查被修改的属性类型
 * 2. 对生命值属性进行范围限制（0到最大生命值）
 * 3. 对怒气值属性进行范围限制（0到最大怒气值）
 * 4. 处理伤害承受逻辑，计算实际生命值变化
 * 5. 输出调试信息
 * 6. 检查角色是否死亡并处理相关逻辑
 * 
 * @note 该函数在GameplayEffect执行后自动调用，用于处理属性变更后的逻辑
 */
void UWarriorAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{

	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	checkf(CachedPawnUIInterface.IsValid(), TEXT("%s didn't implement IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel())

	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();

	checkf(PawnUIComponent, TEXT("%s's PawnUIComponent is nullptr"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	// 检查被修改的属性是否为当前生命值属性
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		// 将当前生命值限制在0到最大生命值之间
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());

		// 设置修正后的生命值
		SetCurrentHealth(NewCurrentHealth);

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());
	}

	// 检查被修改的属性是否为当前怒气值属性
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		// 将当前怒气值限制在0到最大怒气值之间
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());

		// 设置修正后的怒气值
		SetCurrentRage(NewCurrentRage);

		if (GetCurrentRage() == GetMaxRage())
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_Full);
		}
		else if (GetCurrentRage() == 0.f)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_None);
		}
		else
		{
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_Full);
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_None);
		}

		
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage()/GetMaxRage());
		}

		
		
	}

	// 检查被修改的属性是否为承受伤害值属性
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		// 获取当前生命值（应用伤害前）
		const float OldHealth = GetCurrentHealth();
		
		// 获取承受的伤害值
		const float DamageDone = GetDamageTaken();
		
		// 计算应用伤害后的新生命值，并限制在0到最大生命值之间
		// 新生命值 = 原生命值 - 伤害值
		const float NewCurrenHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());

		// 设置新的生命值
		SetCurrentHealth(NewCurrenHealth);

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());
		
		// 输出调试信息，显示伤害计算过程
		// const FString Msg = FString::Printf(TEXT("OldHealth: %f, DamageDone: %f, NewCurrentHealth: %f"),
		// 	OldHealth, DamageDone, NewCurrenHealth);

		

		// Debug::Print(Msg, FColor::Blue);
		
		// 检查角色是否死亡（生命值为0）
		if (GetCurrentHealth() <= 0.f)
		{
			// 调试：检查 AnimInstance 是否有效
			// if (ACharacter* Character = Cast<ACharacter>(Data.Target.GetAvatarActor()))
			// {
			// 	USkeletalMeshComponent* Mesh = Character->GetMesh();
			// 	UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;
			// 	
			// 	UE_LOG(LogTemp, Warning, TEXT("[Death Check] Mesh Valid: %s, AnimInstance Valid: %s"),
			// 		Mesh ? TEXT("YES") : TEXT("NO"),
			// 		AnimInstance ? TEXT("YES") : TEXT("NO"));
			// }
			//
			// UE_LOG(LogTemp, Error, TEXT("[Death] Adding Death Tag to Actor: %s"), *Data.Target.GetAvatarActor()->GetName());
			
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),
				WarriorGameplayTags::Shared_Status_Dead);
			
			// UE_LOG(LogTemp, Error, TEXT("[Death] Death Tag Added Successfully"));
			
			
		}
	}
}