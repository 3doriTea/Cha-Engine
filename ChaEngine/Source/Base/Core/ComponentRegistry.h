#pragma once

namespace cha
{
	/// @brief コンポーネント登録
	class ComponentRegistry
	{
	private:
		ComponentRegistry() = delete;
		~ComponentRegistry() = delete;

	public:
		/// @brief コンポーネントの登録インデクスを取得する
		/// @tparam ComponentT コンポーネント型
		/// @return 登録インデクス
		template<typename ComponentT>
		inline static uint64_t GetComponentIndex()
		{
			static const uint64_t INDEX{ IndexCounter()++ };
			return INDEX;
		}

	private:
		/// @brief 登録インデクスのカウンタを参照
		/// @return カウンタの参照ポインタ
		inline static uint64_t& IndexCounter()
		{
			static uint64_t counter{ 0 };
			return counter;
		}
	};
}
