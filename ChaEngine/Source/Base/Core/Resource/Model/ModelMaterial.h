#pragma once
#include "../../../BasePch.h"
#include "../Material/MaterialProperty.h"

namespace cha
{
	struct ModelMaterialProperty_
	{
		Mat4x4 toWorldMatrix{ Mat4x4::Identity() };
		Int4 hasTextureSkinAnim{ FALSE, FALSE, FALSE, FALSE };
		Float4 baseColor{};
	};

	using ModelMaterialProperty = MaterialProperty<ModelMaterialProperty_>;
}
