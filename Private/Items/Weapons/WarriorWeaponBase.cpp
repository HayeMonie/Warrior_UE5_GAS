// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"
#include "WarriorFunctionLibrary.h"

#include "WarriorDebugHelper.h"


/**
 * @brief 构造函数实现
 * 
 * 设置武器Actor的默认属性值
 * 初始化武器网格体和碰撞盒组件
 * 配置碰撞事件处理
 * 
 * @details
 * 1. 禁用Actor的Tick功能以提高性能
 * 2. 创建并设置武器网格体组件为根组件
 * 3. 创建并配置武器碰撞盒组件
 * 4. 注册碰撞事件处理函数
 */
AWarriorWeaponBase::AWarriorWeaponBase()
{
 	// 禁用Actor的Tick功能以提高性能
	// 对于不需要每帧更新的武器Actor，禁用Tick可以提升游戏性能
	PrimaryActorTick.bCanEverTick = false;

	// 创建武器网格体组件
	// CreateDefaultSubobject用于在构造函数中创建组件，确保组件在编辑器和运行时都正确初始化
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	
	// 设置武器网格体为根组件
	// SetRootComponent设置Actor的根组件，所有其它组件都将相对于此组件定位
	SetRootComponent(WeaponMesh);

	// 创建武器碰撞盒组件
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	
	// 将碰撞盒组件附加到根组件
	// SetupAttachment用于将组件附加到另一个组件上
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	
	// 设置碰撞盒的大小
	// SetBoxExtent设置盒形碰撞体的大小，参数为各方向的半长
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	
	// 默认禁用碰撞检测
	// ECollisionEnabled::NoCollision表示不进行碰撞检测
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 注册碰撞开始重叠事件处理函数
	// AddUniqueDynamic用于动态绑定UFUNCTION标记的函数到委托
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	
	// 注册碰撞结束重叠事件处理函数
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
}

/**
 * @brief 碰撞盒开始重叠事件处理函数实现
 * 
 * 当武器碰撞盒开始与其它组件重叠时调用
 * 检查重叠对象是否为有效目标，如果是则触发击中事件
 * 
 * @param OverlappedComponent 发生重叠的组件（武器碰撞盒）
 * @param OtherActor 与之重叠的其它演员
 * @param OtherComp 与之重叠的其它组件
 * @param OtherBodyIndex 其它物理体的索引
 * @param bFromSweep 是否为扫掠结果
 * @param SweepResult 扫掠结果信息
 * 
 * @details
 * 1. 获取武器的拥有者（Pawn）
 * 2. 检查重叠对象是否为Pawn
 * 3. 确保重叠对象不是武器拥有者本身
 * 4. 触发武器击中目标事件
 */
void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 获取武器的拥有者Pawn
	// GetInstigator用于获取导致此Actor生成的Pawn
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	// 检查武器拥有者是否有效，无效则触发带格式化信息的断言
	checkf(WeaponOwningPawn, TEXT("Weapon Owning Pawn is null"))

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
	


}

/**
 * @brief 碰撞盒结束重叠事件处理函数实现
 * 
 * 当武器碰撞盒结束与其它组件重叠时调用
 * 检查重叠对象是否为有效目标，如果是则触发拔出事件
 * 
 * @param OverlappedComponent 结束重叠的组件（武器碰撞盒）
 * @param OtherActor 与之结束重叠的其它演员
 * @param OtherComp 与之结束重叠的其它组件
 * @param OtherBodyIndex 其它物理体的索引
 * 
 * @details
 * 1. 获取武器的拥有者（Pawn）
 * 2. 检查重叠对象是否为Pawn
 * 3. 确保重叠对象不是武器拥有者本身
 * 4. 触发武器从目标拔出事件
 */
void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 获取武器的拥有者Pawn
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	
	// 检查武器拥有者是否有效，无效则触发带格式化信息的断言
	checkf(WeaponOwningPawn, TEXT("Weapon Owning Pawn is null"))

	// 检查重叠对象是否为Pawn类型
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}

	// TODO: Implement hit check for enemy character
	// 待办：实现对敌人角色的命中检查
	
}