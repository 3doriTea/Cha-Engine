#pragma once
#include <Game/Game.h>

/// @brief プレイヤーの状態
struct PlayerState
{
	bool onGrounded{};  ///< 地面に着地しているか

	float velocityY{};  ///< 速度 Y軸
	Vec3 velocity{};    ///< 速度

	float previousHeight{};  ///< 前のフレームでのプレイヤーの高さ

	bool isActiveControl{ false };  ///< 操作可能か true / false

	Quaternion targetRotation{};  ///< 自動回転の目標回転

	float hoverPositionY{ 0.0f };  ///< ホバリング目標座標y

	float toStartHoveringTimeLeft{ 0.3f };  ///< ホバリング開始するまでの押し込み時間計測用

	int32_t jumpCount{ 0 };  ///< 現在のジャンプカウント
};
