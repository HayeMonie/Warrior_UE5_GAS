// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorDebugHelper.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SaveGame/WarriorSaveGame.h"
#include "WarriorTypes/WarriorCountDownAction.h"

/**
 * 从指定Actor获取Warrior能力系统组件的原生函数实现
 * @param InActor 需要获取能力系统组件的Actor，不能为空
 * @return 返回Cast后的UWarriorAbilitySystemComponent指针
 */
UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

/**
* 1. 动态生命周期 Loose标签是临时性标签，通过AddLooseGameplayTag添加后，其生命周期由开发者手动管理（通过RemoveLooseGameplayTag显式移除），或通过AddLooseGameplayTagTimed设置自动过期时间。
* 2. 与基标签分离 Loose标签独立于对象的基标签集（Base Tags），后者通常存储在UAbilitySystemComponent的BaseTags成员中，而Loose标签存储在ActiveTags的LooseTags容器里。
* 3. 引用计数机制 Loose标签采用引用计数管理（通过FGameplayTagCount结构），允许多次添加相同标签（引用计数递增），需匹配次数的移除操作才会彻底删除。
 */

/**
 * 用于管理Actor上的Gameplay标签
 * 如果Actor上没有指定标签，则添加该标签
 * @param InActor 需要添加标签的Actor
 * @param TagToAdd 需要添加的Gameplay标签
 */
void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (ASC -> HasMatchingGameplayTag(TagToAdd) == false)    // 没有找到对应的标签
	{
		ASC -> AddLooseGameplayTag(TagToAdd);     // 则添加临时标签
	}
	
}

/**
 * 如果Actor上有指定标签，则移除该标签
 * @param InActor 需要移除标签的Actor
 * @param TagToRemove 需要移除的Gameplay标签
 */
void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	
	if (ASC -> HasMatchingGameplayTag(TagToRemove))    // 找到对应的标签
	{
		ASC -> RemoveLooseGameplayTag(TagToRemove);     // 则移除标签
	}
}

/**
 * 原生函数：检查Actor是否具有指定的游戏标签
 * @param InActor 需要检查标签的Actor
 * @param TagToCheck 需要检查的Gameplay标签
 * @return 如果Actor具有指定标签返回true，否则返回false
 */
bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	return ASC -> HasMatchingGameplayTag(TagToCheck);

	
}

/**
 * 蓝图函数：检查Actor是否具有指定的游戏标签，使用枚举扩展执行引脚
 * @param InActor 需要检查标签的Actor
 * @param TagToCheck 需要检查的Gameplay标签
 * @param OutConfirmResult 输出结果枚举，根据是否有标签返回Yes或No
 */
void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmResult)
{
	NativeDoesActorHaveTag(InActor, TagToCheck) ? OutConfirmResult = EWarriorConfirmType::Yes : OutConfirmResult = EWarriorConfirmType::No;

	
}

/**
 * 从Actor获取Pawn战斗组件的原生函数实现
 * @param InActor 需要获取战斗组件的Actor，不能为空
 * @return 返回UPawnCombatComponent指针，如果不存在则返回nullptr
 */
UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
	
}

/**
 * 蓝图函数：从Actor获取Pawn战斗组件，使用有效性枚举扩展执行引脚
 * @param InActor 需要获取战斗组件的Actor
 * @param OutValidType 输出结果枚举，根据组件是否有效返回Valid或Invalid
 * @return 返回UPawnCombatComponent指针
 */
UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;

	return CombatComponent;
	
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	
	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}

	return false;
	
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirection(AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	if (CrossResult.Z < 0.0f)
	{
		OutAngleDifference *= -1.0f;
	}

	if (OutAngleDifference >= -45.0f && OutAngleDifference <= 45.0f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < 45.f && OutAngleDifference >= -135.0f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.0f || OutAngleDifference > 135.0f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.0f && OutAngleDifference <= 135.0f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}
	
	return WarriorGameplayTags::Shared_Status_HitReact_Front;

	
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	// const FString DebugString = FString::Printf(TEXT("Dot Result: %f %s"), DotResult, DotResult < -0.1f ? TEXT("Blocked") : TEXT("Not Blocked"));
	//
	// Debug::Print(DebugString, DotResult< -0.1f ? FColor::Green : FColor::Red);
	
	return DotResult < -0.1f;

	
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC -> ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime,
	EWarriorCountDownActionInput CountDownInput, UPARAM(DisplayName = "Output") EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;

	if (GEngine)
	{
		World = GEngine -> GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return;
	}

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	if (CountDownInput == EWarriorCountDownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
				new FWarriorCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo));
		}
	}

	if (CountDownInput == EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction -> CancelAction();
		}
	}
	
}

UWarriorGameInstance* UWarriorFunctionLibrary::GetWarriorGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine -> GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World -> GetGameInstance<UWarriorGameInstance>();
		}
	}

	return nullptr;
}

void UWarriorFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode)
{
	APlayerController* PlayerController = nullptr;

	if (GEngine)
	{
		if (UWorld* World = GEngine -> GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}

		if (!PlayerController)
		{
			return;
		}

		FInputModeGameOnly GameOnlyMode;
		FInputModeUIOnly UIOnlyMode;

		switch (InInputMode)
		{
			case EWarriorInputMode::GameOnly:
				{
					PlayerController -> SetInputMode(GameOnlyMode);
					PlayerController -> bShowMouseCursor = false;
				}
		
				break;
			case EWarriorInputMode::UIOnly:
				{
					PlayerController -> SetInputMode(UIOnlyMode);
					PlayerController -> bShowMouseCursor = true;
				}
			break;
		}
	}
}

void UWarriorFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave)
{
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());

	if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
	{
		WarriorSaveGameObject->SavedCurrentGameDifficulty = InDifficultyToSave;

		const bool bWasSaved =UGameplayStatics::SaveGameToSlot(WarriorSaveGameObject, WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString() ,0);

		Debug::Print(bWasSaved ? TEXT("Saved Successfully") : TEXT("Saved Failed"));
	}
}

bool UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSavedDifficulty)
{
	if (UGameplayStatics::DoesSaveGameExist(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0))
	{
		USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
		{
			OutSavedDifficulty = WarriorSaveGameObject->SavedCurrentGameDifficulty;

			// Debug::Print(TEXT("Loaded Successfully"));

			return true;
		}
	}

	return false;
}
