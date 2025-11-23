// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorHeroCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WarriorInputComponent.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"


#include "WarriorDebugHelper.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "GameModes/WarriorBaseGameMode.h"


/**
 * @brief 构造函数实现
 * 
 * 设置英雄角色的默认属性值
 * 初始化摄像机组件、战斗组件等
 * 
 * @details
 * 1. 配置角色碰撞胶囊体大小
 * 2. 设置控制器旋转参数
 * 3. 创建并配置摄像机系统组件
 * 4. 配置角色移动组件参数
 * 5. 创建英雄战斗组件
 */
AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	// 初始化角色碰撞胶囊体大小(半径43，高度96)
	// 胶囊体用于角色的碰撞检测和物理交互
	GetCapsuleComponent()->InitCapsuleSize(43.f, 96.f);

	// 禁用控制器旋转，让角色移动时自动朝向移动方向
	// 这样可以实现更自然的第三人称控制体验
	bUseControllerRotationPitch = false;  // 禁用俯仰旋转
	bUseControllerRotationYaw = false;    // 禁用偏航旋转
	bUseControllerRotationRoll = false;   // 禁用翻滚旋转
	
	// 创建摄像机摇臂组件，用于第三人称视角
	// 摄像机摇臂实现了摄像机与角色之间的弹性连接
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	
	// 将摄像机摇臂附加到角色的根组件上
	CameraBoom->SetupAttachment(GetRootComponent());
	
	// 设置摄像机摇臂的长度，控制摄像机与角色的距离
	CameraBoom->TargetArmLength = 200.f;
	
	// 设置摄像机摇臂的偏移量，调整摄像机相对于角色的位置
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
	
	// 设置摄像机摇臂跟随控制器旋转
	CameraBoom->bUsePawnControlRotation = true;

	// 创建跟随摄像机组件
	// 这是玩家实际看到的摄像机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	
	// 将摄像机附加到摄像机摇臂的末端
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	// 摄像机不直接使用pawn控制旋转，而是通过摇臂间接控制
	FollowCamera->bUsePawnControlRotation = false;

	// 配置角色移动组件
	// 设置角色移动时自动朝向移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// 设置角色旋转速率，仅在偏航轴上旋转（水平方向）
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	
	// 设置最大行走速度
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	
	// 设置行走时的制动减速度
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// 创建英雄战斗组件
	// 用于处理英雄特有的战斗逻辑
	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));

	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));

	
}

/**
 * @brief 获取角色战斗组件实现
 * 
 * 实现IPawnCombatInterface接口，返回英雄的战斗组件
 * 
 * @return 返回英雄的战斗组件指针
 * 
 * @see UHeroCombatComponent
 */
UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
	// 返回英雄战斗组件
	return HeroCombatComponent;
}

UPawnUIComponent* AWarriorHeroCharacter::GetPawnUIComponent() const
{
	
	return HeroUIComponent;
}

UHeroUIComponent* AWarriorHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

/**
 * @brief 角色被控制器占有时的回调函数实现
 * 
 * 当角色被玩家控制器占有时调用
 * 用于初始化能力系统和应用启动数据
 * 
 * @param NewController 占有角色的新控制器（玩家控制器）
 * 
 * @details
 * 1. 调用父类的PossessedBy函数
 * 2. 同步加载并应用启动数据到能力系统组件
 */
void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	// 调用父类的PossessedBy函数，确保基础功能正常执行
	Super::PossessedBy(NewController);

	// 确保能力系统组件有效后再继续
	if (WarriorAbilitySystemComponent)
	{
		// 如果角色启动数据不为空，则加载并应用到能力系统组件
		if (!CharacterStartUpData.IsNull())
		{
			// 同步加载启动数据资产
			// LoadSynchronous会阻塞直到资源加载完成，适用于初始化阶段
			if(UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
			{
				int32 AbilityApplyLevel = 1;

				if (AWarriorBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AWarriorBaseGameMode>())
				{
					switch (BaseGameMode->GetCurrentGameDifficulty())
					{
					case EWarriorGameDifficulty::Easy:
						AbilityApplyLevel = 4;
						break;
					case EWarriorGameDifficulty::Normal:
						AbilityApplyLevel = 3;
						break;
					case EWarriorGameDifficulty::Hard:
						AbilityApplyLevel = 2;
						break;
					case EWarriorGameDifficulty::Hell:
						AbilityApplyLevel = 1;
						break;
					}	
				}
				
				// 将加载的数据应用到能力系统组件
				// GiveToAbilitySystemComponent是数据资产中的方法，用于初始化角色的能力系统
				LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);
			}
		}
	}
}

/**
 * @brief 设置玩家输入组件实现
 * 
 * 重写父类函数，用于绑定玩家输入动作
 * 配置增强输入系统和输入映射
 * 
 * @param PlayerInputComponent 玩家输入组件指针
 * 
 * @details
 * 1. 验证输入配置数据资产的有效性
 * 2. 获取并配置增强输入子系统
 * 3. 绑定移动、视角和能力输入动作
 */
void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 检查是否分配了有效的输入配置数据资产
	// checkf是带格式化字符串的断言，如果条件不满足会输出错误信息并中断程序
	checkf(InputConfigDataAsset, TEXT("Forget to assign a valid data asset as input config"));
	
	// 获取本地玩家对象，用于访问输入子系统
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	
	// 获取增强输入子系统
	// ULocalPlayer::GetSubsystem用于获取与本地玩家关联的子系统实例
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	// 验证子系统是否有效
	check(Subsystem);

	// 添加默认的输入映射上下文
	// 输入映射上下文定义了输入动作与处理函数之间的映射关系
	// 第二个参数0表示优先级，数值越小优先级越高
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	// 转换为Warrior输入组件类型
	// CastChecked是安全的类型转换，如果转换失败会触发断言
	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	// 绑定移动输入动作
	// 使用输入配置数据资产中的配置，将InputTag_Move标签与Input_Move函数绑定
	// ETriggerEvent::Triggered表示触发事件（持续按住）
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Move,
		ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	
	// 绑定视角输入动作
	// 将InputTag_Look标签与Input_Look函数绑定
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Look,
		ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_SwitchTarget,
		ETriggerEvent::Triggered, this, &ThisClass::Input_SwitchTargetTriggered);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_SwitchTarget,
		ETriggerEvent::Completed, this, &ThisClass::Input_SwitchTargetCompleted);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_PickUp_Stones,
		ETriggerEvent::Started, this, &ThisClass::Input_PickUpStonesStarted);
	
	// 绑定能力输入动作(按下和释放)
	// 将能力输入标签与对应的按下和释放处理函数绑定
	// 这允许处理复杂的输入事件，如技能释放
	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);

	
}

/**
 * @brief 游戏开始时调用实现
 * 
 * 重写父类函数，在游戏开始时执行初始化逻辑
 * 确保角色的旋转和移动设置正确
 */
void AWarriorHeroCharacter::BeginPlay()
{
	// 调用父类的BeginPlay函数，确保基础功能正常执行
	Super::BeginPlay();
       
	// 确保这些设置不被其他地方覆盖
	// 重复设置控制器旋转参数，确保第三人称控制的正确性
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false; 
	
	// 确保角色移动时自动朝向移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// 确保角色旋转速率设置正确
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
}

/**
 * @brief 处理角色移动输入实现
 * 
 * 处理玩家的移动输入，控制角色在世界中的移动
 * 
 * @param InputActionValue 输入动作的值，包含移动方向信息
 * 
 * @details
 * 1. 获取二维移动向量
 * 2. 计算基于控制器旋转的移动方向
 * 3. 分别处理前后和左右移动输入
 */
void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	// 获取二维移动向量(X轴和Y轴)
	// FVector2D的X分量表示左右移动，Y分量表示前后移动
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	
	// 获取控制器的yaw旋转(水平旋转)，用于确定移动方向
	// 仅使用偏航旋转，忽略俯仰和翻滚
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

	// 处理前后移动(Y轴输入)
	if (MovementVector.Y != 0.f)
	{
		// 计算向前方向向量
		// RotateVector将向量按照指定旋转进行变换
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		
		// 添加前后移动输入
		// AddMovementInput是角色移动的基础函数
		AddMovementInput(ForwardDirection, MovementVector.Y);
	} 

	// 处理左右移动(X轴输入)
	if (MovementVector.X != 0.f)
	{
		// 计算向右方向向量
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

		// 添加左右移动输入
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

/**
 * @brief 处理视角控制输入实现
 * 
 * 处理玩家的视角控制输入，控制摄像机的视角方向
 * 
 * @param InputActionValue 输入动作的值，包含视角控制方向信息
 * 
 * @details
 * 1. 获取二维视角控制向量
 * 2. 分别处理水平和垂直视角旋转
 */
void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	// 获取视角控制的二维向量
	// FVector2D的X分量表示水平旋转，Y分量表示垂直旋转
	const FVector2D LookAxisVector =  InputActionValue.Get<FVector2D>();

	// 处理水平视角旋转(X轴输入)
	// AddControllerYawInput控制控制器的水平旋转
	if(LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	// 处理垂直视角旋转(Y轴输入)
	// AddControllerPitchInput控制控制器的垂直旋转
	if(LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWarriorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2D>();
}

void AWarriorHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,
		SwitchDirection.X > 0.f ? WarriorGameplayTags::Player_Event_SwitchTarget_Right : WarriorGameplayTags::Player_Event_SwitchTarget_Left,
		Data);

	
}

void AWarriorHeroCharacter::Input_PickUpStonesStarted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,
		WarriorGameplayTags::Player_Event_ConsumeStones,
		Data
	);
}

/**
 * @brief 处理能力输入按下事件实现
 * 
 * 处理玩家按下能力相关输入时的逻辑
 * 通知能力系统组件处理输入按下事件
 * 
 * @param InInputTag 与按下输入相关联的GameplayTag
 * 
 * @details
 * 将输入事件转发给能力系统组件处理
 */
void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	// 通知能力系统组件输入被按下
	// OnAbilityInputPressed会查找与输入标签匹配的能力并尝试激活
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

/**
 * @brief 处理能力输入释放事件实现
 * 
 * 处理玩家释放能力相关输入时的逻辑
 * 通知能力系统组件处理输入释放事件
 * 
 * @param InInputTag 与释放输入相关联的GameplayTag
 * 
 * @details
 * 将输入事件转发给能力系统组件处理
 */
void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	// 通知能力系统组件输入被释放
	// OnAbilityInputReleased会处理能力的释放逻辑
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}