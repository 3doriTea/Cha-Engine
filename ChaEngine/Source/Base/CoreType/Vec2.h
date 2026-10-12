#pragma once
#include <DirectXMath.h>
#include "Vec2T.h"

namespace cha
{
	/// @brief 浮動小数点型2次元ベクトルのトレイト
	struct Vec2Traits
	{
		using T = float_t;
		using DirectXMathT = DirectX::XMVECTOR;
		using D2D1T = D2D1_VECTOR_2F;
		using BIGT = double_t;
	};

	/// @brief 浮動小数点型2次元ベクトル
	using Vec2 = Vec2T<Vec2Traits>;
}
