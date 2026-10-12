#pragma once
#include <Base/Base.h>
#include "../Component/Transform.h"

/// @brief 座標系の更新処理システムのデータ
struct UITranslationSystemData
{
	std::set<GameObject> checked_{};
	std::vector<GameObject> calculationQueue_{};
};

/// @brief 座標系の更新処理システム
struct UITranslationSystem : SystemObjectBase<UITranslationSystem, UITranslationSystemData>
{
	/// @brief 更新処理
	void Update();
};
