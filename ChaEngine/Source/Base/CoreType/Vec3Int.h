#pragma once
#include <DirectXMath.h>
#include "Vec3T.h"

namespace cha
{
	/// @brief 整数型3次元ベクトルのトレイト
	struct Vec3IntTraits
	{
		using T = int32_t;
		using DirectXMathT = DirectX::XMINT3;
		using D2D1T = D2D1_POINT_2L;
		using BIGT = int64_t;
	};

	/// @brief 整数型3次元ベクトル
	using Vec3Int = Vec3T<Vec3IntTraits>;
}
