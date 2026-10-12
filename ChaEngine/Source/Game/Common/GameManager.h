#pragma once
#include <Base/Base.h>
#include "SceneType.h"

/// @brief ゲームシーン問わず共通する処理を行うマネージャのデータ
struct GameManagerData
{
};

/// @brief ゲームシーン問わず共通する処理を行うマネージャ
struct GameManager : ObjectBase<GameManagerData>
{
	/// @brief 最初に１度だけ呼ばれる処理
	void Start();

	/// @brief 毎フレームの更新処理
	void Update();
};
