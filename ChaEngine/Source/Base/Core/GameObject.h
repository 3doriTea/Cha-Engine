#pragma once

namespace cha
{
	/// @brief ゲームオブジェクト (中身は数値)
	/// @note IsEmptyはオブジェクト未参照の状態、boolキャストはオブジェクトがシーンに存在するかも確認する
	class GameObject
	{
		friend class GameObjectCollection;

	public:
		/// @brief ゲームオブジェクトを初期化
		GameObject() = default;
		/// @brief ゲームオブジェクトを初期化
		/// @param _id オブジェクトId
		/// @param _pGameObjectCollection ゲームオブジェクトコレクションのポインタ
		GameObject(const uint64_t _id, GameObjectCollection* _pGameObjectCollection);
		/// @brief 解放処理
		~GameObject();

		/// @brief ゲームオブジェクトのIdを取得
		/// @return シーン内の識別番号
		inline const uint64_t GetId() const { return id_; }

		/// @brief 空っぽのゲームオブジェクトを取得する
		/// @return 空っぽのゲームオブジェクト
		static inline GameObject Empty() { return GameObject{ 0, nullptr }; }

		/// @brief ゲームオブジェクトが空っぽであるか NOTE: オブジェクトがシーンに存在するかは関係しないため処理は軽い
		/// @return 空っぽである true / false
		inline bool IsEmpty() const { return id_ == 0 || pGameObjectCollection_ == nullptr; }

		/// @brief 暗黙的にbool変換 -> オブジェクトを参照でき、シーンに存在するか true / false
		operator bool() const;

	private:
		uint64_t id_{ 0ULL };  ///< 識別番号
		GameObjectCollection* pGameObjectCollection_{ nullptr };   ///< 所属するオブジェクトコレクションの参照
	};
}

// stdに追加する
namespace std
{
	template <>
	struct hash<cha::GameObject>
	{
		size_t operator()(const cha::GameObject& _gameObject) const
		{
			// メンバ変数のハッシュ値を返す
			size_t h1 = std::hash<uint64_t>{}(_gameObject.GetId());
			return h1;
		}
	};
}
