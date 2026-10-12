#pragma once
#include "../BasePch.h"
#include "GameObject.h"
#include "ObjectBase.h"
#include "IObject.h"
#include "ISystemObject.h"
#include "Component.h"
#include "GameObjectElement.h"

namespace cha
{
	template<typename T>
	concept ObjectT = std::is_base_of_v<IObject, T>;

	template<typename DataT>
	class ObjectBase;

	class ComponentPools;

	/// @brief ゲームオブジェクトを持つ
	class GameObjectCollection
	{
	public:
		/// @brief ゲームオブジェクトコレクションの初期化処理
		GameObjectCollection();
		/// @brief ゲームオブジェクトコレクションの解放処理
		~GameObjectCollection();

		/// @brief システムオブジェクトを生成する
		/// @tparam ...Args 可変長引数型
		/// @tparam SystemObjectT システムオブジェクトの型
		/// @param ..._args 可変長引数
		/// @return 追加したゲームオブジェクト
		template<std::derived_from<ISystemObject> SystemObjectT, typename ...Args>
		GameObject Add(Args&&... _args);

		/// @brief ゲームオブジェクトを生成する
		/// @return 追加したゲームオブジェクト
		template<typename ObjectT, typename ...Args>
		GameObject Add(Args&&... _args);

		/// @brief ゲームオブジェクトを削除予定にする
		/// @param _gameObject ゲームオブジェクト
		void DestroyAt(const GameObject _gameObject);

		/// @brief ゲームオブジェクトをクリア
		void Clear();

		/// @brief 指定のゲームオブジェクトが存在するかを取得
		/// @param _gameObject 指定のゲームオブジェクト
		/// @return 存在する true / false
		bool Exist(const GameObject _gameObject) const;
		
		/// @brief 指定のゲームオブジェクトが存在するかを取得
		/// @param _id 指定のゲームオブジェクトのId
		/// @return 存在する true / false
		bool Exist(const uint64_t _id) const;

		/// @brief ゲームオブジェクトを取得
		/// @tparam T 
		/// @param _gameObject 
		/// @return 
		template<typename ObjectT = ObjectBase<>>
		ObjectT& At(const GameObject _gameObject);

		/// @brief ゲームオブジェクトを取得
		/// @tparam T 
		/// @param _id 
		/// @return 
		template<typename ObjectT>
		ObjectT& At(const uint64_t _id);

		/// @brief 指定ゲームオブジェクトが所有するコンポーネントを見つける
		/// @tparam ComponentT コンポーネントの型
		/// @param _gameObject ゲームオブジェクト
		/// @return コンポーネントを所有していればポインタ / 見つからなければ nullptr
		template<typename ComponentT>
		ComponentT* FindComponent(const GameObject _gameObject);

		/// @brief コンポーネントを見つける
		/// @tparam ComponentT コンポーネントの型
		/// @return コンポーネントが見つかればコンポーネントのポインタ / 見つからなければ nullptr
		template<typename ComponentT>
		ComponentT* FindComponent(const uint64_t _id);

		/// @brief レジストリ内インデックスからの速い参照
		/// @param _index インデックス
		/// @tparam Func ラムダ式
		/// @param _callback コールバック処理
		/// @return 
		template<typename Func>
		void AtFast(const uint64_t _index, Func&& _fanc);

		/// @brief すべてのオブジェクトを処理する
		/// @tparam Func ラムダ式
		/// @param _callback コールバック処理
		template<typename Func>
		void ForEach(Func&& _callback);

		/// @brief システムオブジェクトを処理する
		/// @tparam Func ラムダ式 ISystemObject&
		/// @param _callback コールバック処理
		template<typename Func>
		void ForEachSystem(Func&& _callback);

		/// @brief 予定されたゲームオブジェクトを処理する
		void Flush();

		/// @brief ビューを取得する
		/// @tparam ...ComponentsT コンポーネント型
		/// @return ビュー
		template<typename ...ComponentsT>
		auto GetView();

		/// @brief オブジェクトIdからコンポーネントを参照する
		/// @tparam ComponentT コンポーネントの型
		/// @param _id オブジェクトId
		/// @return コンポーネントの参照ポインタ
		template<typename ComponentT>
		ComponentT& GetComponent(const uint64_t _id);

		/// @brief ゲームオブジェクトを作成する
		/// @param _id オブジェクトId
		/// @return ゲームオブジェクト
		GameObject MakeGameObject(const uint64_t _id);

		/// @brief 総オブジェクト数を取得する
		/// @return 総オブジェクト数
		inline size_t GetSize() const { return registry_.size(); }

	private:
		/// @brief ゲームオブジェクトを削除する
		/// @param _id 削除するゲームオブジェクトId
		void Remove(const uint64_t _id);

		/// @brief ゲームオブジェクトIdをレジストリのインデクスへ変換する
		/// @param _id ゲームオブジェクトのId
		/// @return レジストリのインデクス
		size_t IdToRegistryIndex(const uint64_t _id) const;

		/// @brief 指定されたインデクスがレジストリ内を指すか
		/// @param _index インデクス
		/// @return インデクスがレジストリ内を指す true / false
		bool IsRegisteredIndex(const size_t _index) const;

		/// @brief 指定されたインデクスが登録予定配列内を指すか
		/// @param _index インデクス
		/// @return インデクスが登録予定配列内を指す true / false
		bool IsPendingIndex(const size_t _index) const;

		/// @brief コンポーネントレジストリを更新する
		/// @param _flag フラグ
		/// @param _gameObjectId ゲームオブジェクトのId
		/// @param _addedIndex 保留枠でのインデックス
		/// @note 内部でId to registry index やっているからここはIdを渡してください！
		void UpdateComponentRegistry(const ComponentFlag _flag, const uint64_t _gameObjectId, const size_t _addedIndex);

		/// @brief コンポーネントレジストリを平らにする
		void FillComponentRegistry();

	private:
		const size_t SYSTEM_COUNT_;  ///< 登録済みのシステム数
		std::vector<std::shared_ptr<ISystemObject>> systemsSubRegistry_{};  ///< システムのサブレジストリ

		std::vector<size_t> indices_{};  ///< ゲームオブジェクトIdはこの配列のインデクスとなり、この配列要素の値はレジストリのインデクスとなる。
		
		std::vector<GameObjectElement> registry_{};  ///< レジストリ
		
		std::vector<GameObjectElement> toAdd_{};  ///< 次のループで登録される予定の要素
		
		std::set<uint64_t> toDestroyIds_{};  ///< 削除予定のゲームオブジェクトId

		std::unique_ptr<ComponentPools> pComponentPools_{};  ///< コンポーネントプール
	};
}

#include "GameObjectCollection.inl"
