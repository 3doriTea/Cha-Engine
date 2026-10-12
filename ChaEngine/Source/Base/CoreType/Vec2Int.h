#pragma once
#include "Vec2T.h"
#include <DirectXMath.h>

namespace cha
{
	/// @brief 整数型2次元ベクトルのトレイト
	struct Vec2IntTraits
	{
		using T = int32_t;
		using DirectXMathT = DirectX::XMINT2;
		using D2D1T = D2D1_VECTOR_2F;
		using BIGT = int64_t;
	};

	/// @brief 整数型2次元ベクトル
	using Vec2Int = Vec2T<Vec2IntTraits>;
}
