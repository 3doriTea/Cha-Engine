#pragma once
#include <Base/Base.h>

/// @brief スプライト描画のマテリアルプロパティ
struct SpriteRendererMaterialProperty_
{
	Int4 hasTexture;  // テクスチャを持っているか
	Color baseColor;  // ベースの色
};

/// @brief スプライト描画のマテリアルプロパティ
using SpriteRendererMaterialProperty = MaterialProperty<SpriteRendererMaterialProperty_>;
