#pragma once
#include <Base/Base.h>

/// @brief UI当たり判定の状態
using UIRaycastTargetState = uint32_t;

/// @brief UI当たり判定の状態
enum UIRaycastTargetState_
{
	UIRaycastTargetState_Disabled,  // 無効
	UIRaycastTargetState_Normal,    // 通常状態
	UIRaycastTargetState_Hovered,   // カーソルが乗っている
	UIRaycastTargetState_Pressed,   // 押されている
};
