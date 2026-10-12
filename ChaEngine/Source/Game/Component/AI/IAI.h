#pragma once
#include <Game/Game.h>

/// @brief AIのインタフェース
class IAI
{
public:
	virtual ~IAI() = default;

	virtual bool TryUpdate() = 0;

	/// @brief 現在目標にしている座標
	/// @return 目標の座標
	virtual Vec3 GetTargetPosition() const = 0;

	/// @brief 現在見つめている対象
	/// @return 見つめている対象 / なければ nullptr
	virtual Vec3 GetLookTarget() const = 0;
};
