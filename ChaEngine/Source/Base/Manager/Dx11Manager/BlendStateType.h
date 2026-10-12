#pragma once
#include "../../BasePch.h"

namespace cha
{
	/// @brief ブレンドステートの種類
	using BlendStateType = uint32_t;

	/// @brief ブレンドステートの種類
	enum BlendStateType_ : BlendStateType
	{
		BlendStateType_None,        ///< ブレンドステート無し
		BlendStateType_Alpha,       ///< 半透明描画する
		BlendStateType_Max,         ///< 最大数
	};
}
