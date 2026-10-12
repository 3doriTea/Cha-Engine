#pragma once
#include <Base/Base.h>
#include "../Component/Transform.h"

/// @brief 座標系の更新処理システムのデータ
struct TranslationSystemData
{
	std::set<GameObject> checked_{};
	std::vector<GameObject> calculationQueue_{};
};

/// @brief 座標系の更新処理システム
struct TranslationSystem : SystemObjectBase<TranslationSystem, TranslationSystemData>
{
	/// @brief 更新処理 全Transformのワールド計算を行う
	/// @memo 任意のタイミングで呼び出し可能
	void Update();
};
