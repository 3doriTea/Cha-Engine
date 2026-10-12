#pragma once
#include <Game/Game.h>

/// @brief 起動シーンのロゴデータ
struct BootSceneLogoData
{
	Resource<Texture> texture{ "ChaengineIcon.png" };                    ///< ロゴの画像テクスチャ
	Component<UITransform> transform{ Vec2::Zero(), Vec2{ 256, 256 }, UILayerOrder{ 100 } };  ///< UI座標系コンポーネント
	Component<UIImage> image{};                                          ///< 画像コンポーネント

	float fadeInTime{ 1.0f };                                  ///< 非表示状態からだんだん表示される時間(秒)
	float stayTime{ 1.0f };                                    ///< はっきり表示される時間(秒)
	float fadeOutTime{ 1.0f };                                 ///< 表示状態からだんだん非表示にされる時間(秒)
	float timeLeftSec{ fadeInTime + stayTime + fadeOutTime };  ///< 残りの表示時間(秒)
};

/// @brief 起動シーンのロゴ
struct BootSceneLogo : ObjectBase<BootSceneLogoData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
};
