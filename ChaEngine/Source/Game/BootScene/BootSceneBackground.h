#pragma once
#include <Game/Game.h>

/// @brief 起動シーンの背景データ
struct BootSceneBackgroundData
{
	Component<UITransform> transform{ Vec2::Zero(), Vec2::Zero(), UILayerOrder{ 20 } };   ///< UI座標系
};

/// @brief 起動シーンの背景
struct BootSceneBackground : ObjectBase<BootSceneBackgroundData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 描画処理
	void Draw() override;
};