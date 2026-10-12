#pragma once
#include "../../BasePch.h"

namespace cha
{
	/// @brief 参照関係を解決する
	class DependencyResolver
	{
	private:
		/// @brief 参照関係
		struct DependencyLink
		{
			inline DependencyLink(const uint64_t _self, std::vector<uint64_t>&& _refers) :
				self{ _self },
				refers{ std::move(_refers) }
			{}

			uint64_t self{};  // 自身
			std::vector<uint64_t> refers{};  // 参照先
		};
	public:
		DependencyResolver() = default;
		~DependencyResolver() = default;

		/// @brief 参照を解決する
		/// @return 解決されたvector - 未解決時はサイズ0
		std::vector<uint64_t> Resolve();

	private:
		std::vector<DependencyLink> links_{};
		std::vector<uint64_t> deadlocked_{};
	};
}
