#pragma once
#include "IScene.h"
#include "GameObject.h"

namespace cha
{
	class GameObjectCollection;

	/// @brief シーンのベースクラス
	class SceneBase : public IScene
	{
	public:
		SceneBase();
		virtual ~SceneBase() = default;

		/// @brief シーン開始時の処理
		inline virtual void Start() override {}

		/// @brief シーンにオブジェクトを作成する
		/// @tparam T オブジェクトの型
		/// @tparam ...Args コンストラクタ可変長引数引数
		/// @param ..._args コンストラクタ可変長引数引数
		/// @return ゲームオブジェクト
		template<typename T, typename ...Args>
		inline GameObject Instantiate(Args&&... _args)
			requires std::constructible_from<T, Args...>;

		/// @brief 指定のオブジェクトを削除する
		/// @param _gameObject ゲームオブジェクト
		void Destroy(GameObject _gameObject);

		/// @brief シーンの更新処理
		virtual void Update() override final;
		/// @brief シーンの遅い更新処理
		virtual void LateUpdate() override final;
		/// @brief シーンの描画処理
		virtual void Draw(const bool _isShadowing) override final;
		/// @brief シーン終了時の処理
		virtual void End() override final;

		/// @brief ゲームオブジェクトコレクションを直接参照する
		/// @return ゲームオブジェクトコレクションの参照
		GameObjectCollection& RefGameObjectCollection() override;

	private:
		// シーン内ゲームオブジェクトを格納する
		std::unique_ptr<GameObjectCollection> pGameObjectCollection_{};
	};
}
