#pragma once
#include <Base/Base.h>

/// @brief UI画像の描画コンポーネント
struct UIImage : ComponentBase<UIImage>
{
	Resource<ResourceType::Texture> texture{};  // テクスチャ
	Color baseColor{};                          // ベースの色
};
