#pragma once
#include "../../BasePch.h"

namespace cha
{
	class GameObjectCollection;
	class IObject;

	/// @brief 指定コンポーネントを持つオブジェクトを周回するビュー
	/// @tparam ...ComponentsT 指定のコンポーネントたち
	template<typename ...ComponentsT>
	class View
	{
	public:
		/// @brief ビューの初期化処理
		/// @param _collection ゲームオブジェクトコレクション
		View(GameObjectCollection& _collection);
		/// @brief ビューの解放処理
		~View() = default;

		/// @brief 条件に一致するオブジェクトを周回する
		/// @tparam Func void(IObject&, ComponentT&...)
		/// @param _func ラムダ式
		template<typename Func>
		void ForEach(Func&& _func);

		/// @brief 条件に一致するオブジェクトをインデックスとともに周回する
		/// @tparam Func void(IObject&, ComponentT&...)
		/// @param _func ラムダ式
		template<typename Func>
		void ForEachWithIndex(Func&& _func);

		/// @brief コレクションへのインデックスで高速アクセス
		/// @tparam Func void(IObject&, ComponentT&...)
		/// @param _index インデックス
		/// @param _func ラムダ式
		template<typename Func>
		void AtFast(const size_t _index, Func&& _func);

		/// @brief 再帰停止用
		/// @param _flag 所有コンポーネントフラグ
		inline void GetComponentFlag(ComponentFlag& _flag) {}

		/// @brief 所有コンポーネントフラグを取得する
		/// @tparam FirstT 最初のコンポーネント型
		/// @tparam ...Rest 続くコンポーネントの型たち
		/// @param _flag 所有コンポーネントフラグ
		template<typename FirstT, typename ...Rest>
		void GetComponentFlag(ComponentFlag& _flag);

		/// @brief 周回するオブジェクト数、indexの総数を取得する
		/// @return 総数
		size_t GetSize() const { return collection_.GetSize(); }

	private:
		GameObjectCollection& collection_;  ///< ゲームオブジェクトコレクションの参照
	};
}

#include "View.inl"
