#pragma once
#include <Game/Game.h>

/// @brief プレイヤーの設定情報
struct PlayerConfig
{
	float gravityPerSec{ 9.8f };  ///< 重力加速度

	float jumpTargetHeight{ 3.0f };        ///< ジャンプできる高さ
	float rotateThreshold{ 0.01f };  ///< 動いたと判断して回転をかけるしきい値

	float moveSpeedPerSec{ 4.0f };   ///< 1秒間に移動する速度
	float hoverSpeedPerSec{ 2.0f };  ///< 1秒間に空中移動する速度

	int32_t jumpCountMax{ 2 };  ///< 何回までジャンプできるのか

	float friction{ 0.9f };  ///< 空気抵抗 0 -> 1に近いほど緩い

	float rotateSpeedPerSec{ 3.0f };  ///< 自動回転スピード(秒)

	float upPushThresholdCos{ 0.9f };  ///< 上向きへの押し出しを判定する cos値 -1 -> +1に近いほど上向き

	float jetPower{ 15.0f };  ///< ジェットの基本出力
	float stability{ 1.0f };  ///< ふわんふわん戻ろうとする強さ
	float hoverDragPerSec{ 1.7f };  ///< 空気抵抗 ブレーキ

	float toStartHoveringTimeSec{ 0.3f };  ///< ホバリングを開始するまでの押し込み時間(秒)

	float stepHeightMax{ 1.0f };  ///< プレイヤーが登れる高さ

	/*NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		PlayerConfig,
		jumpTargetHeight,
		rotateThreshold,
		moveSpeedPerSec,
		hoverSpeedPerSec,
		jumpCountMax,
		friction,
		upPushThresholdCos);*/
};
