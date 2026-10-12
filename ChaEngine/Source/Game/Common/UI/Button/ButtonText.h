#pragma once
#include <Game/Game.h>

namespace UI
{
	/// @brief ボタンテキストUIオブジェクトのデータ
	struct ButtonTextData
	{
		Component<UITransform> transform{};   // UI座標系
		Component<UIText> text{};             // テキスト
	};

	/// @brief ボタンUIオブジェクト
	struct ButtonText : ObjectBase<ButtonTextData>
	{
		using ObjectBase<ButtonTextData>::ObjectBase;
	};
}
