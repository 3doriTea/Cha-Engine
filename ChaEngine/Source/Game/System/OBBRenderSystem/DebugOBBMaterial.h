#pragma once
#include <Base/Base.h>

/// @brief デバッグのOBB表示マテリアルプロパティ
struct DebugOBBMaterialProperty_
{
	Mat4x4 transform{ Mat4x4::Identity() };
};

/// @brief デバッグのOBB表示マテリアルプロパティ
using DebugOBBMaterialProperty = MaterialProperty<DebugOBBMaterialProperty_>;
