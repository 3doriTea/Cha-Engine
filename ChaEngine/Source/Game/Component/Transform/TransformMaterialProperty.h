#pragma once
#include <Base/Base.h>

/// @brief 座標系のマテリアルプロパティ
struct TransformMaterialProperty_
{
	Mat4x4 world{ Mat4x4::Identity() };   // ワールド行列
	Mat4x4 normal{ Mat4x4::Identity() };  // 回転のみのワールド行列
};

/// @brief 座標系のマテリアルプロパティ
using TransformMaterialProperty = MaterialProperty<TransformMaterialProperty_>;
