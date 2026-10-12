#pragma once
#include "NullObjectData.h"
#include "ISystemObject.h"
#include "../Manager/IManager.h"
#include "IObject.h"
#include "IComponent.h"
#include "../Core/ComponentPool/View.h"


namespace cha
{
	class IManager;
	class ISystemObject;
	class SceneManager;
	class SceneBase;

	/// @brief オブジェクトの基底クラス
	/// @tparam DataT 所有データ
	template<typename DataT = NullObjectData>
	class ObjectBase : protected DataT, public IObject
	{
		friend class GameObjectCollection;
	public:
		using DataType = DataT;

	public:
		/// @brief オブジェクトデータ構造体のコンストラクタ引数から初期化する
		/// @tparam ...Args オブジェクトデータ構造体のコンストラクタ引数の型
		/// @param ..._args オブジェクトデータ構造体のコンストラクタ引数
		template<typename ...Args>
		ObjectBase(Args&& ..._args) requires std::constructible_from<DataT, Args...>;

		ObjectBase(DataT&& _data);
		ObjectBase(const DataT& _data);

		virtual ~ObjectBase() = default;

		/// @brief インスタンス時に呼ばれる処理
		inline virtual void Start() override {}
		/// @brief 毎フレームの更新処理
		inline virtual void Update() override {}
		/// @brief 毎フレームの描画処理
		inline virtual void Draw() override {}
		/// @brief 毎フレームのシャドウマップ描画処理
		inline virtual void DrawShadowMap() override {}
		/// @brief ゲームオブジェクト終了時の処理
		inline virtual void OnDestroy() override {}


		/// @brief 読み取り専用でデータを参照する
		/// @return const 参照ポインタ
		const DataT& Data() const { return *this; }
		
		/// @brief データを参照する
		/// @return const 参照ポインタ
		DataT& Data() { return *this; }

		/// @brief ゲームオブジェクトへ変換
		/// @return ゲームオブジェクト
		GameObject ToGameObject() const override;

		/// @brief このゲームオブジェクトがアクティブかを取得
		/// @return アクティブである true / false
		inline const bool IsActive() const override { return isActive_; }

		/// @brief このゲームオブジェクトがアクティブか設定する
		/// @param _isActive アクティブである true / false
		inline void SetActive(const bool _isActive) override { isActive_ = _isActive; }

		/// @brief このゲームオブジェクトを削除する
		inline void DestroyMe();
	
	protected:
		/// @brief 指定のシステムオブジェクトを参照する
		/// @tparam T システムオブジェクトの型
		/// @return システムオブジェクトの参照
		template<std::derived_from<ISystemObject> T>
		inline T& Get();

		/// @brief 指定のマネージャを参照する
		/// @tparam T 指定のマネージャ型
		/// @return マネージャの参照
		template<std::derived_from<IManager> T>
		inline T& Get();

		/// @brief 指定コンポーネントを含む全てのオブジェクトを参照するビューを取得する
		/// @tparam ...ComponentsT 指定のコンポーネント
		/// @return ビュー
		template<typename ...ComponentsT>
		inline View<ComponentsT...> GetView();

		/// @brief このオブジェクトが所属するシーンを参照する
		/// @tparam SceneT シーンの型 (未指定でSceneBase型)
		/// @return シーンの参照
		template<typename SceneT = SceneBase>
		inline SceneT& GetScene() const;

		/// @brief オブジェクトに含まれるコンポーネントを参照する
		/// @tparam T コンポーネントの型
		/// @param _gameObject オブジェクトのハンドル
		/// @return コンポーネントの参照 (無ければエラー)
		/// @note オブジェクトが必ずコンポーネントを含んでいる確証がない場合は、 TryGet を使ってください。
		template<std::derived_from<IComponent> T>
		inline T& Get(const GameObject _gameObject);

		/// @brief 試しにオブジェクトに含まれるコンポーネントを参照する
		/// @tparam T 試しに取得したいコンポーネントの型
		/// @param _gameObject ゲームオブジェクトのハンドル
		/// @return コンポーネントのポインタ / 参照できなければ nullptr
		template<std::derived_from<IComponent> T>
		inline T* TryGet(const GameObject _gameObject);
		
		/// @brief 同じシーンのゲームオブジェクトを参照する
		/// @tparam T オブジェクトの型(インタフェース可)
		/// @param _gameObject ゲームオブジェクトのハンドル
		/// @return オブジェクトの参照
		template<typename T>
		inline T& Get(const GameObject _gameObject);

	private:
		/// @brief オブジェクトのIdを取得する
		/// @return オブジェクトのId
		inline const uint64_t GetId() const override { return id_; }
		/// @brief オブジェクトのIdをセットする
		/// @param _id オブジェクトのId
		inline void SetId(const uint64_t _id) override { id_ = _id; }

	private:
		uint64_t id_{ 0 };       ///< オブジェクトに割り当てられるId
		bool isActive_{ true };  ///< このゲームオブジェクトがアクティブかのフラグ
	};
}

#include "ObjectBase.inl"
