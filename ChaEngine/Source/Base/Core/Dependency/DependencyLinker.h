#pragma once
#include "../../BasePch.h"
#include "../SystemRegistry.h"

namespace cha
{
	/// @brief 依存関係を登録、リンクするやつ
	class DependencyLinker
	{
		friend class GameObjectCollection;
	public:
		DependencyLinker() = default;
		~DependencyLinker() = default;

		/// @brief 参照を追加する
		/// @tparam SystemObjectT 参照先
		/// @return 自身の参照ポインタ
		template<typename SystemObjectT>
		inline DependencyLinker& ReferTo()
		{
			const uint64_t INDEX{ SystemRegistry::GetSystemIndex<SystemObjectT>() };
			referObjectIds_.insert(INDEX);

			return *this;
		}

	private:
		// 参照先のオブジェクトId
		std::set<uint64_t> referObjectIds_{};
	};
}
