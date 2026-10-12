#pragma once
#include <Base/Base.h>

/// @brief UIの座標系マテリアルプロパティ
struct UITransformMaterialProperty_
{
	Mat4x4 ncd{ Mat4x4::Identity() };  // ncd座標系へ変換する行列
};

/// @brief UIの座標系マテリアルプロパティ
using UITransformMaterialProperty = MaterialProperty<UITransformMaterialProperty_>;
