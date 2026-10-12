#pragma once
#include "../Core/IScene.h"
#include "IGameLoopCallable.h"
#include "../Core/GameObject.h"
#include "../Core/SystemRegistry.h"
#include "IManager.h"
#include "../Core/ComponentPool/View.h"

namespace cha
{
	class IScene;

	/// @brief シーンを管理する
	class SceneManager : public IGameLoopCallable, public IManager
	{
	public:
		SceneManager();
		~SceneManager();

		/// @brief シーンを遷移
		/// @tparam T 遷移先のシーン
		template<typename T>
		inline void Move()
		{
			if (pMakeNextSceneFactory_)
			{
				VCU_ASSERT(false && "既にシーン遷移予定が入っている");
				return;
			}
			pMakeNextSceneFactory_ = std::make_unique<std::function<void()>>([this]()
			{
				pCurrentScene_ = std::make_unique<T>();
			});
		}

		/// @brief 現在のシーンを終了する
		inline void Clear()
		{
			if (pCurrentScene_)
			{
				pCurrentScene_->End();
				pCurrentScene_.reset();
			}
		}

		/// @brief 現在のシーンのポインタを取得
		/// @return シーンの参照ポインタ
		cha::SceneBase& GetCurrentScene();

		/// @brief ゲームオブジェクトを見つける
		/// @tparam T 
		/// @param _gameObject 
		/// @return 
		template<typename T>
		inline T& FindObject(const GameObject _gameObject)
		{
			return pCurrentScene_->RefGameObjectCollection().template At<T>(_gameObject);
		}

		/// @brief ゲームオブジェクトが所有するコンポーネントを見つける
		/// @tparam T コンポーネントの型
		/// @param _gameObject コンポーネントが所属するゲームオブジェクト
		/// @return コンポーネント
		template<std::derived_from<IComponent> T>
		inline T* FindComponent(const GameObject _gameObject)
		{
			return pCurrentScene_->RefGameObjectCollection().template FindComponent<T>(_gameObject);
		}

		template<typename T>
		inline T& GetSystemObject()
		{
			return pCurrentScene_->RefGameObjectCollection().template At<T>(SystemRegistry::GetSystemIndex<T>());
		}

		template<typename ...ComponentsT>
		inline View<ComponentsT...> GetView()
		{
			return pCurrentScene_->RefGameObjectCollection().template GetView<ComponentsT...>();
		}

		GameObject MakeGameObject(const uint64_t _id);

		/// @brief シーン遷移が発生するか
		inline bool HasNextScene() const { return !!pMakeNextSceneFactory_; }

		/// @brief 指定のゲームオブジェクトを次のフレームで消す
		/// @param _gameObject 削除予定のゲームオブジェクト
		void DestroyAt(const GameObject _gameObject);

	private:
		bool Update() override;
		bool LateUpdate() override;
		bool Draw(const bool _isShadowing) override;
		bool Refresh() override;

	private:
		std::unique_ptr<IScene> pCurrentScene_{};                         // 現在のシーン
		std::unique_ptr<std::function<void()>> pMakeNextSceneFactory_{};  // 次のシーンを作成する無名関数
	};
}

#include "SceneManager.inl"
