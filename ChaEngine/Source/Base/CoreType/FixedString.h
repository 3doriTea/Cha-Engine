#pragma once
#include <algorithm>

namespace cha
{
	/// @brief 固定長文字列
	/// @tparam N 文字数
	template<size_t N>
	struct FixedString
	{
		char buffer[N];
		constexpr FixedString(const char(&_str)[N])
		{
			std::copy_n(_str, N, buffer);
		}
	};
}
