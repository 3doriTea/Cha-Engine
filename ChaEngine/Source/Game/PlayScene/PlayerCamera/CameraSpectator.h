#pragma once
#include <Game/Game.h>
#include "IPlayerCamera.h"

/// @brief カメラ: 観戦者モード のデータ
struct CameraSpectatorData
{
	float moveSpeedPerSec{ 3.0f };    ///< 1秒間に移動する速度
	float rotateSpeedPerSec{ 3.0f };  ///< 1秒間に回転する角度

	PlayerCameraCommon common{};  ///< 共有設定
	bool isActive{ false };       ///< 有効であるか
};

/// @brief カメラ: 観戦者モード
struct CameraSpectator : ObjectBase<CameraSpectatorData>, IPlayerCamera
{
	/// @brief 開始処理
	void Start();
	/// @brief 更新処理
	void Update();

	/// @brief 有効化する
	/// @param _common 引継ぎ共通項目
	void Activate(const PlayerCameraCommon& _common) override;
	/// @brief 無効化する
	/// @return 引継ぎ共通項目
	PlayerCameraCommon Deactivate() override;
};