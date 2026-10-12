#pragma once
#include <Base/Base.h>
#include <Base/Core/Resource/Material/MaterialProperty.h>

/// @brief シェーダ時間マテリアルのプロパティ
struct TimeMaterialProperty_
{
	Float4 time{};  // 時間
};

/// @brief シェーダ時間マテリアルのプロパティ
using TimeMaterialProperty = MaterialProperty<TimeMaterialProperty_>;
