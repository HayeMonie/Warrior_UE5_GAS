/**
* @切记！！切记！！切记！！ 所有的Attribute名称不要进行二次修改！！！不然会引发大量的联动错误！！！
 * @切记！！切记！！切记！！ 且引擎会因为底层代码不好修改而疯狂报错
 * @切记！！切记！！切记！！ 总之：请勿进行属性名称的二次修改！！！
 */


#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/PawnUIInterface.h"
#include "WarriorAttributeSet.generated.h"

// 宏定义的作用：自动为属性生成一组标准的访问器函数
// 用于Unreal Gameplay Ability System（GAS）属性的读取、设置和初始化，减少重复代码。
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
* @切记！！切记！！切记！！ 所有的Attribute名称不要进行二次修改！！！不然会引发大量的联动错误！！！
 * @切记！！切记！！切记！！ 且引擎会因为底层代码不好修改而疯狂报错
 * @切记！！切记！！切记！！ 总之：请勿进行属性名称的二次修改！！！
 */

/*
 * 
 */
UCLASS()
class WARRIOR_API UWarriorAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 构造函数
	UWarriorAttributeSet();

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	// 当前生命值
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentHealth)

	// 最大生命值
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxHealth)

	// 当前怒气值
	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentRage)

	// 最大怒气值
	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxRage)

	// 攻击力
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, AttackPower)

	// 防御力
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DefensePower)

	// 伤害承受值
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DamageTaken)

private:
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

	
	
};