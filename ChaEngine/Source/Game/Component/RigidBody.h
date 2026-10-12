#pragma once
#include <Base/Base.h>

/// @brief 物理挙動するコンポーネント
struct RigidBody : ComponentBase<RigidBody>
{
	Vec3 previousWorldPosition{};  // 前回のフレームでのワールド座標
	Vec3 velocity{};  // 速度
};
