#pragma once
#include <Game/Game.h>

/// @brief プレイヤーの共通インタフェース
struct IPlayer
{
	/// @brief 解放処理 仮想
	virtual ~IPlayer() = default;

	/// @brief プレイヤーの操作を有効にするかを設定
	/// @param _isActive 操作を有効 true / false
	virtual void SetActiveControl(const bool _isActive) = 0;

	/// @brief 自動回転目標を設定する
	/// @param _targetRotation 目標の回転
	virtual void SetTargetRotation(const Quaternion& _targetRotation) = 0;
};
