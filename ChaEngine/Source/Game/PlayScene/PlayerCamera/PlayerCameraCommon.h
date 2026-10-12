#pragma once
#include <Game/Game.h>

/// @brief プレイヤーのカメラに共通し引き継がれる項目
struct PlayerCameraCommon
{
	Vec3 position{};  ///< 座標
	Vec2 angles{};    ///< 回転角度
};
