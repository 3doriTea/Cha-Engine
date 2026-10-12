#pragma once
#include <Game/Game.h>

/// @brief UIのレイキャストシステム
struct UIRaycasterSystemData
{

};

/// @brief UIのレイキャストシステム
struct UIRaycasterSystem : SystemObjectBase<UIRaycasterSystem, UIRaycasterSystemData>
{
	/// @brief 毎フレームの更新処理
	void Update() override;
};
