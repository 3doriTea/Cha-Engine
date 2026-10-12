#pragma once
#include <Base/Base.h>
#include <Base/Core/Resource/Material/MaterialProperty.h>

/// @brief カメラマテリアルのプロパティ
struct CameraMaterialProperty_
{
	// ビュープロジェクション行列
	Mat4x4 viewProjectionMatrix{ Mat4x4::Identity() };
	Float4 position{};  // カメラの座標
};

/// @brief カメラマテリアルのプロパティ
using CameraMaterialProperty = MaterialProperty<CameraMaterialProperty_>;
