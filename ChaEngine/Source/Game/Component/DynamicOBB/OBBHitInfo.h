#pragma once

/// @brief OBBが当たったときの情報
struct OBBHitInfo
{
	GameObject gameObject{};  // ゲームオブジェクト
	Vec3 push{};  // 押し出された量
};
