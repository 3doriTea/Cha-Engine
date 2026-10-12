#pragma once
#include <Base/Base.h>
#include "UIRaycastTarget/UIRaycastTargetState.h"

/// @brief UIがマウスカーソルに反応するようにするターゲット
struct UIRaycastTarget : ComponentBase<UIRaycastTarget>
{
	UIRaycastTargetState currentState{ UIRaycastTargetState_Normal };  ///< 現在のステート
	bool isPressedThisFrame{ false };                                  ///< UIが押された瞬間か
	bool isReleasedThisFrame{ false };                                 ///< UIが押されて離された瞬間か
};
