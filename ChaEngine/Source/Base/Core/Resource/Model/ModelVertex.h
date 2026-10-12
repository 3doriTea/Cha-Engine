#pragma once
#include "../../../BasePch.h"

namespace cha
{
	/// @brief モデルの頂点情報
	struct alignas(16) ModelVertex
	{
		Float4 position;  // 頂点座標
		Float4 normal;    // 法線
		Float2 uv;        // UV座標

		// アニメーション用
		uint32_t boneIndices[4];  // 影響を受けるボーンのインデックス(最大4つ)
		float boneWeights[4];     // 各ボーンの影響度・重み(最大4つ、合計値が必ず 1.0f になる)
	};
}
