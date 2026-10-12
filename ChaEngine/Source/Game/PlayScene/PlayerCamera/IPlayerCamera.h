#pragma once
#include "PlayerCameraCommon.h"

/// @brief プレイヤーカメラ共通のインタフェース
struct IPlayerCamera
{
	/// @brief 解放処理 仮想
	virtual ~IPlayerCamera() = default;

	/// @brief 共通項目を与えて有効化する
	/// @param _common 有効化する際の引継ぎ共通項目
	virtual void Activate(const PlayerCameraCommon& _common) = 0;

	/// @brief 無効化し時点での共通項目を取得し無効化する
	/// @return カメラ共通設定
	virtual PlayerCameraCommon Deactivate() = 0;
};
