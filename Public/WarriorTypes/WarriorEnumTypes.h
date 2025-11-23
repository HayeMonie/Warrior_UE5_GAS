#pragma once

// 定义确认类型枚举，用于表示是/否的选择结果
// 主要用于函数返回值，配合ExpandEnumAsExecs元数据在蓝图中扩展执行引脚
UENUM()
enum class EWarriorConfirmType_Enum : uint8
{
	Yes,  // 确认/是 - 对应蓝图中的"是"执行引脚
	No    // 否定/否 - 对应蓝图中的"否"执行引脚
};

// 定义有效性类型枚举，用于表示对象是否有效
// 主要用于检查对象是否存在或是否有效，配合ExpandEnumAsExecs元数据在蓝图中扩展执行引脚
UENUM()
enum class EWarriorValidType : uint8
{
	Valid,   // 有效 - 对应蓝图中的"有效"执行引脚
	Invalid  // 无效 - 对应蓝图中的"无效"执行引脚
};

UENUM()
enum class EWarriorSuccessType : uint8
{
	Successful,  
	Failed  
};

UENUM()
enum class EWarriorCountDownActionInput : uint8
{
	Start,
	Cancel
};

UENUM()
enum class EWarriorCountDownActionOutput : uint8
{
	Updated,
	Completed,
	Cancelled
};

UENUM(BlueprintType)
enum class EWarriorGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	Hell
};

UENUM(BlueprintType)
enum class EWarriorInputMode : uint8
{
	GameOnly,
	UIOnly,
};



