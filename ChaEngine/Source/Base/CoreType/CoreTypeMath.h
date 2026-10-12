#pragma once
#include <cmath>
#include <algorithm>

namespace cha::CoreTypeMath
{
	/// @brief 2点間を線形補間をする
	/// @tparam T 同じ型
	/// @param _a 開始値
	/// @param _b 終了値
	/// @param _t 補間値
	/// @return _a から _b を _t 値で線形補間
	template<typename T>
	inline static T Lerp(const T _a, const T _b, const float _t)
	{
		float t = std::clamp(_t, 0.0f, 1.0f);

		// tがほぼ0以下なら_aを返す 0割りを阻止
		if (t <= FLT_EPSILON)
		{
			return _a;
		}

		return (_b - _a) * t + _a;
	}
}
