#pragma once
#include <Base/Core/ComponentBase.h>
#include "StaticOBB.h"
#include "DynamicOBB/OBBHitInfo.h"

/// @brief 動く側のOBB
struct DynamicOBB : ComponentBase<DynamicOBB, StaticOBB>
{
	//Vec3 position;                     // 位置
	//std::array<Vec3, 3> normalDirect;  // 各方向へのベクトル
	//Vec3 axisLength;                   // 各軸方向への長さ
	//Vec3 offset{};              // ユーザー側設定
	//Vec3 scale{ Vec3::One() };  // ユーザー側設定

	bool onGrounded{ false };                  ///< 接地フラグ
	std::vector<OBBHitInfo> hitGameObjects{};  ///< 当たっているOBBの情報
};
