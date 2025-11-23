// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

/**
 * @brief 目标交互委托声明
 * 
 * 声明一个带有一个AActor*参数的委托
 * 用于在武器与目标发生交互时通知监听者
 * 
 * @param AActor* 与武器发生交互的目标演员
 */
DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*)

/**
 * @brief 基础武器类
 * 
 * 继承自AActor，作为所有武器的基类
 * 提供武器的基本功能，包括网格体、碰撞检测和交互事件
 * 
 * @details
 * 1. 包含武器静态网格体组件
 * 2. 包含武器碰撞盒组件
 * 3. 处理碰撞事件
 * 4. 提供武器与目标交互的委托
 * 
 * @see AActor
 */
UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	/**
	 * @brief 构造函数
	 * 
	 * 设置武器Actor的默认属性值
	 * 初始化武器网格体和碰撞盒组件
	 */
	AWarriorWeaponBase();

	/**
	 * @brief 武器击中目标委托
	 * 
	 * 当武器击中目标时触发的委托
	 * 可以绑定多个处理函数来响应击中事件
	 */
	FOnTargetInteractedDelegate OnWeaponHitTarget;
	
	/**
	 * @brief 武器从目标拔出委托
	 * 
	 * 当武器从目标拔出时触发的委托
	 * 可以绑定多个处理函数来响应拔出事件
	 */
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;

protected:
	/**
	 * @brief 武器网格体组件
	 * 
	 * 武器的可视化网格体组件
	 * 作为武器的根组件
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	UStaticMeshComponent* WeaponMesh;
	
	/**
	 * @brief 武器碰撞盒组件
	 * 
	 * 用于检测武器与其它物体的碰撞
	 * 默认不启用碰撞检测，需要时手动启用
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	UBoxComponent* WeaponCollisionBox;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
public:
	/**
	 * @brief 获取武器碰撞盒组件
	 * 
	 * 内联函数，用于快速获取武器碰撞盒组件
	 * 
	 * @return 返回武器碰撞盒组件指针
	 */
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const
	{
		return WeaponCollisionBox;
	}

	

};

