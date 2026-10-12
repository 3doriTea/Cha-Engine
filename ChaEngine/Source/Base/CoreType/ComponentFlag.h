#pragma once
#include <bitset>
#include <cstdint>

namespace cha
{
	/// @brief コンポーネントの最大数
	inline static constexpr size_t COMPONENT_CAPACITY{ sizeof(uint64_t) * 8 };

	/// @brief コンポーネントの所有フラグ
	using ComponentFlag = std::bitset<COMPONENT_CAPACITY>;
}
