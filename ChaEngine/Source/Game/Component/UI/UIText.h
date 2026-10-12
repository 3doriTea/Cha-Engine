#pragma once
#include <Base/Base.h>

/// @brief スプライトの描画コンポーネント
struct UIText : ComponentBase<UIText>
{
	UIText(const std::string_view _content = "") :
		content{ _content }
	{
	}

	std::string content{};  ///< 表示したいテキスト
};
