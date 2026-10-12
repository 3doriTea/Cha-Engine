#pragma once
#include "Mat4x4.h"
#include "Vec3.h"

namespace cha
{
	/// @brief Vec3と行列をかける
	/// @param _v 3次元浮動小数点ベクトル
	/// @param _m 変換行列
	/// @return 変換された3次元浮動小数点ベクトル
	inline Vec3 operator*(const Vec3& _v, const Mat4x4& _m)
	{
		using namespace DirectX;

		return Vec3{ XMVector3TransformCoord(_v, _m) };
	}
}
