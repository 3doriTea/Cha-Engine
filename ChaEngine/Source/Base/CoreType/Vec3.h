#pragma once
#include <DirectXMath.h>
#include "Vec3T.h"

namespace cha
{
	/// @brief 浮動小数点型3次元ベクトル
	struct Vec3Traits
	{
		using T = float_t;
		using DirectXMathT = DirectX::XMVECTOR;
		using D2D1T = D2D1_VECTOR_2F;
		using BIGT = double_t;
	};

	/// @brief 浮動小数点型3次元ベクトル
	using Vec3 = Vec3T<Vec3Traits>;
}
