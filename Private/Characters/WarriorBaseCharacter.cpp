// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorBaseCharacter.h"

/**
 * @brief 构造函数实现
 * 
 * 设置角色的默认属性值
 * 初始化能力系统组件和属性集组件
 * 
 * @details
 * 1. 禁用角色的Tick功能以提高性能
 * 2. 设置角色网格体不接收 decals（贴花）
 * 3. 创建并初始化能力系统组件
 * 4. 创建并初始化属性集组件
 */
AWarriorBaseCharacter::AWarriorBaseCharacter()
{
 	// 禁用角色的Tick功能以提高性能
	// 对于不需要每帧更新的角色，禁用Tick可以提升游戏性能
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 设置角色网格体不接收 decals（贴花）
	// decals通常用于血迹、弹孔等效果，禁用可以提高渲染性能
	GetMesh()->bReceivesDecals = false;

	// 创建默认子对象：战士能力系统组件
	// CreateDefaultSubobject用于在构造函数中创建组件，确保组件在编辑器和运行时都正确初始化
	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));

	// 创建默认子对象：战士属性集组件
	// 属性集组件包含角色的所有属性，如生命值、怒气值等
	WarriorAttributeSet = CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	
}

/**
 * @brief 获取能力系统组件实现
 * 
 * 实现IAbilitySystemInterface接口，返回角色的能力系统组件
 * 通过调用GetWarriorAbilitySystemComponent获取实际组件
 * 
 * @return 返回角色的能力系统组件指针
 * 
 * @see GetWarriorAbilitySystemComponent
 */
UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	// 返回战士能力系统组件
	// 通过内联函数快速获取组件引用
	return GetWarriorAbilitySystemComponent();
}

/**
 * @brief 获取角色战斗组件实现
 * 
 * 实现IPawnCombatInterface接口，返回角色的战斗组件
 * 当前基础角色类未实现具体战斗组件，返回nullptr
 * 
 * @return 返回角色的战斗组件指针，当前实现返回nullptr
 * 
 * @see UPawnCombatComponent
 */
UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	// 基础角色类未实现具体战斗组件，返回空指针
	// 具体的战斗功能将在派生类中实现
	return nullptr;
}

UPawnUIComponent* AWarriorBaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

/**
 * @brief 角色被控制器占有时的回调函数实现
 * 
 * 当角色被AI控制器或玩家控制器占有时调用
 * 用于初始化能力系统和应用启动数据
 * 
 * @param NewController 占有角色的新控制器
 * 
 * @details
 * 1. 调用父类的PossessedBy函数
 * 2. 初始化能力系统组件的Actor信息
 * 3. 将属性集注册到能力系统组件
 * 4. 检查并确保启动数据已分配
 */
void AWarriorBaseCharacter::PossessedBy(AController* NewController)
{
	// 调用父类的PossessedBy函数，确保基础功能正常执行
	Super::PossessedBy(NewController);

	// 检查能力系统组件是否存在
	if (WarriorAbilitySystemComponent)
	{
		// 初始化能力Actor信息
		// 第一个参数是拥有能力的Actor，第二个参数是AvatarActor（通常为角色本身）
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		// 确保AttributeSet已正确注册到AbilitySystemComponent
		// 这是解决Attribute.Get()断言失败的关键步骤
		// 检查属性集组件是否存在
		if (WarriorAttributeSet)
		{
			// 将属性集作为子对象添加到能力系统组件中
			// 确保属性集在能力系统中正确注册，以便能够应用GameplayEffect
			WarriorAbilitySystemComponent->AddAttributeSetSubobject(WarriorAttributeSet);
		}

		// 检查启动数据是否已分配，如果没有则触发调试断言
		// 使用ensureMsgf提供更详细的错误信息，帮助调试
		/** 以下为三种debug断点信息输出方式，这里使用第二种 **/
		// ensure(!CharacterStartUpData.IsNull());
		
		// 确保角色启动数据已分配，如果没有分配则输出详细错误信息
		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forget to Assign Start up Data to %s"), *GetName());

		// if (ensure(!CharacterStartUpData.IsNull()));
	}
}