// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/Input/DataAsset_InputConfig.h"

/**
 * @brief 根据标签查找原生输入动作实现
 * 
 * 在原生输入动作配置数组中查找与指定标签匹配的输入动作
 * 
 * @param InInputTag 要查找的输入标签
 * @return 找到的输入动作对象指针，未找到则返回nullptr
 * 
 * @details
 * 1. 遍历原生输入动作配置数组
 * 2. 检查每个配置项的标签是否与目标标签匹配
 * 3. 检查配置项的输入动作对象是否有效
 * 4. 如果匹配成功则返回对应的输入动作对象
 * 5. 遍历完成后未找到匹配项则返回nullptr
 */
UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag InInputTag) const
{
	// 遍历原生输入动作配置数组
	for (const FWarriorInputActionConfig& InputActionConfig : NativeInputActions)
	{
		// 检查当前配置项的标签是否与目标标签匹配，并且输入动作对象有效
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			// 返回匹配的输入动作对象
			return InputActionConfig.InputAction;
		}
	}
	
	// 未找到匹配的输入动作，返回空指针
	return nullptr;
}