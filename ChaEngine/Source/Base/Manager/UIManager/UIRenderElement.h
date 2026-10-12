#pragma once
#include <Base/Base.h>
#include "UILayerOrder.h"

namespace cha
{
	/// @brief UIとして描画する要素
	struct UIRenderElement
	{
		UILayerOrder order{};           ///< 描画優先度オーダー
		std::function<void()> func{};   ///< 描画処理
		bool isD2D{ false };            ///< このレンダラーが Direct2D を使うか (true の場合 D2D Begin/End を管理)
	};
}
