#pragma once
#include "../../../BasePch.h"
#include "../Material/MaterialProperty.h"

namespace cha
{
	/// @brief モデル用のTransformマテリアルプロパティ
	struct ModelTransformMaterialProperty_
	{
		Mat4x4 toWorldMatrix{ Mat4x4::Identity() };
	};
	
	/// @brief モデル用のTransformマテリアルプロパティ
	using ModelTransformMaterialProperty = MaterialProperty<ModelTransformMaterialProperty_>;
}
