#pragma once
#include <Game/Game.h>

/// @brief ライトマテリアルのプロパティ
struct LightMaterialProperty_
{
	Mat4x4 worldViewProjection{ Mat4x4::Identity() };
};

/// @brief ライトマテリアルのプロパティ
using LightMaterialProperty = MaterialProperty<LightMaterialProperty_>;
