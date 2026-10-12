#pragma once
#include <DirectXMath.h>
#include "Vec2T.h"

namespace cha
{
	/// @brief float 2つ分
	struct Float2Traits
	{
		using T = float_t;
		using DirectXMathT = DirectX::XMFLOAT2;
		using D2D1T = D2D1_VECTOR_2F;
		using BIGT = double_t;
	};

	/// @brief float 2つ分
	using Float2 = Vec2T<Float2Traits>;
}
