#pragma once
#include <Base/Base.h>

struct TerrainMaterialProperty_
{
	Int4 hasTexture{ TRUE, FALSE, FALSE, FALSE, };
	Float4 baseColor{};
};

using TerrainMaterialProperty = MaterialProperty<TerrainMaterialProperty_>;
