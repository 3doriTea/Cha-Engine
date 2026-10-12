#pragma once
#include <Game/Game.h>

#include "../Component/UI/UIText.h"
#include "LifeChecker.h"

/// @brief シーンテスターのデータ
struct SceneTesterData
{
	std::string currentScene{};

	std::unique_ptr<LifeChecker> pLifeChecker{ std::make_unique<LifeChecker>() };

	Component<UITransform> transform{};
	Component<UIText> text{};

	Component<Name> name{ "SceneTester" };  ///< 名前コンポーネント
	Component<Transform> transform3D{};     ///< 座標系

	//Component<ImTransform> imTransform{};
};

/// @brief シーンテスター
struct SceneTester : ObjectBase<SceneTesterData>
{
	using ObjectBase<SceneTesterData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;
};
