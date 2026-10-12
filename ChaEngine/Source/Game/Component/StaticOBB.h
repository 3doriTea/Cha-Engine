#pragma once
#include <Base/Core/ComponentBase.h>

/// @brief 動かない側のOBB
struct StaticOBB : ComponentBase<StaticOBB>
{
	StaticOBB(
		const Vec3 _offset,
		const Vec3 _scale) :
		offset{ _offset },
		scale{ _scale }
	{}

	StaticOBB() :
		StaticOBB{ Vec3::Zero(), Vec3::One() }
	{}

	Vec3 position{};                     // 位置
	std::array<Vec3, 3> normalDirect{};  // 各方向へのベクトル
	Vec3 axisLength{};                   // 各軸方向への長さ
	Vec3 offset{};              // ユーザー側設定
	Vec3 scale{ Vec3::One() };  // ユーザー側設定
	bool isTrigger{ false };    // 当たっても反動がないセンサーとして使うか
};
