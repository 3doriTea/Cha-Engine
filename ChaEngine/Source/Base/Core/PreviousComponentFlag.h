#pragma once
#include "../BasePch.h"
#include "../CoreType/ComponentFlag.h"

namespace cha
{
	/// @brief オブジェクトインスタンス時の所有コンポーネントチェック用
	class PreviousComponentFlag
	{
		friend class GameObjectCollection;
		template<typename T>
		friend class Component;
	public:
		/// @brief オブジェクトインスタンス時の所有コンポーネントチェック用の初期化処理
		PreviousComponentFlag() = delete;
		/// @brief オブジェクトインスタンス時の所有コンポーネントチェック用の解放処理
		~PreviousComponentFlag() = delete;

	private:
		static inline ComponentFlag flag_{};  ///< コンポーネントフラグ
	};
}
