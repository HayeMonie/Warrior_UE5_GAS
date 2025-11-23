// 对敌人角色的默认初始化设置

#include "Characters/WarriorEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "Engine/AssetManager.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"

#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "GameModes/WarriorBaseGameMode.h"
#include "Widgets/WarriorWidgetBase.h"

/**
 * @brief 构造函数实现
 * 
 * 设置敌人角色的默认属性值
 * 配置AI控制和移动相关参数
 * 初始化敌人战斗组件
 * 
 * @details
 * 1. 设置自动AI控制模式
 * 2. 配置角色移动参数，使角色能够自动面向移动方向
 * 3. 创建并初始化敌人战斗组件
 */
AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	// 设置角色自动被AI控制器控制
	// PlacedInWorldOrSpawned表示在世界中放置或生成时自动被AI控制
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 默认禁用控制器旋转，避免角色随控制器旋转而旋转
	bUseControllerRotationPitch = false;  // 禁用俯仰旋转
	bUseControllerRotationRoll = false;   // 禁用翻滚旋转
	bUseControllerRotationYaw = false;    // 禁用偏航旋转

	// 配置角色移动组件参数
	// 让角色的移动组件不再根据控制器的期望旋转来旋转角色
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	
	// 启用面向移动方向，使角色自动朝向移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// 设置旋转速率，仅在偏航轴上旋转（水平方向）
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	
	// 设置最大行走速度
	// 默认值，可以在DataAsset_EnemyStartUpData中覆盖
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	
	// 设置制动减速度
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.0f;

	// 创建默认子对象：敌人战斗组件
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");

	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");

	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHealthWidgetComponent");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
	
	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	
}

/**
 * @brief 角色被控制器占有时的回调函数实现
 * 
 * 当角色被AI控制器占有时调用
 * 用于初始化敌人启动数据
 * 
 * @param NewController 占有角色的新控制器（通常为AI控制器）
 * 
 * @details
 * 1. 调用父类的PossessedBy函数
 * 2. 初始化敌人启动数据
 */
void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	// 调用父类的PossessedBy函数，确保基础功能正常执行
	Super::PossessedBy(NewController);

	// 初始化敌人启动数据
	InitEnemyStartUpData();
}

#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionAttachmentBoneName))
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionAttachmentBoneName);
	}

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionAttachmentBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionAttachmentBoneName);
	}
}
#endif

/**
 * @brief 获取角色战斗组件实现
 * 
 * 实现IPawnCombatInterface接口，返回敌人的战斗组件
 * 
 * @return 返回敌人的战斗组件指针
 * 
 * @see UEnemyCombatComponent
 */
UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	// 返回敌人战斗组件
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}

	
}

void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
}

/**
 * @brief 初始化敌人启动数据实现
 * 
 * 异步加载并应用敌人的启动数据资产
 * 包括能力、属性等初始化配置
 * 
 * @details
 * 1. 检查启动数据是否有效
 * 2. 使用资源管理器异步加载数据资产
 * 3. 加载完成后应用数据到能力系统组件
 */
void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	// 检查角色启动数据是否为空，为空则直接返回
	if (CharacterStartUpData.IsNull())
	{
		return;
	}

	int32 AbilityApplyLevel = 1;

	if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
	{
		switch (BaseGameMode->GetCurrentGameDifficulty())
		{
		case EWarriorGameDifficulty::Easy:
			AbilityApplyLevel = 1;
			break;
		case EWarriorGameDifficulty::Normal:
			AbilityApplyLevel = 2;
			break;
		case EWarriorGameDifficulty::Hard:
			AbilityApplyLevel = 3;
			break;
		case EWarriorGameDifficulty::Hell:
			AbilityApplyLevel = 4;
			break;
		}	
	}

	// 使用Unreal的资源管理器异步加载 CharacterStartUpData 指定的资源（通常是 DataAsset）
	// 异步加载避免阻塞游戏主线程，提高性能
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		// 获取软引用资源的路径，用于异步加载
		CharacterStartUpData.ToSoftObjectPath(),
		
		// 创建一个异步加载完成时的 Lambda 回调函数，等资源加载完成后自动调用
		FStreamableDelegate::CreateLambda(
			[this, AbilityApplyLevel]()
			{
				// 在异步加载完成后，获取并使用实际的数据资产对象指针并调用数据资产方法
				// 如果没有获取到数据资产对象指针，则说明加载失败或数据资产未设置
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					// 调用数据资产的方法，把能力（如技能、属性等）赋予当前角色的能力系统组件
					// GiveToAbilitySystemComponent是数据资产中的方法，用于初始化角色的能力系统
					LoadedData -> GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);

					// Debug
					// Debug::Print(TEXT("Enemy StartUpData loaded and applied successfully!"), FColor::Green, 5.0f);
				}
			}
		)
	);
}