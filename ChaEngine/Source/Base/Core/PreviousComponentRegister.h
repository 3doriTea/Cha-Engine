#pragma once
#include "../BasePch.h"

namespace cha
{
	class IComponent;
	class ComponentPools;

	/// @brief オブジェクトインスタンス時の所有コンポーネント登録用
	class PreviousComponentRegister
	{
		friend class GameObjectCollection;
		template<typename T>
		friend class Component;

	public:
		/// @brief オブジェクトインスタンス時の所有コンポーネント登録用の初期化処理
		PreviousComponentRegister() = delete;
		/// @brief オブジェクトインスタンス時の所有コンポーネント登録用の解放処理
		~PreviousComponentRegister() = delete;

	private:
		static inline std::vector<
			std::array<std::function<void(ComponentPools&, const size_t)>, COMPONENT_CAPACITY>> registry_{};  ///< 登録する関数を入れていく配列, void(コンポーネントプール参照, オブジェクトId) のキュー
	};
}
