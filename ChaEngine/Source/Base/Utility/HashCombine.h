#pragma once

namespace cha
{
	using HashValue = uint64_t;

	/// @brief ハッシュをつなげていく関数
	/// @tparam T プリミティブ型
	/// @param _v 対象の参照
	/// @param _seed シード値 ここに格納される
	template<typename T>
	inline void HashCombine(HashValue& _seed, const T& _v)
	{
		// MEMO: boostにあるhash_combineから持って来た
		std::hash<T> hasher{};
		_seed ^= hasher(_v) + 0x9e3779b97f4a7c15 + (_seed << 6) + (_seed >> 2);
	}
}
