#pragma once
#include <Base/Base.h>

struct SkyMaterialProperty_
{
	Mat4x4 transform{ Mat4x4::Identity() };
	Float4 time{};
};

using SkyMaterialProperty = MaterialProperty<SkyMaterialProperty_>;
