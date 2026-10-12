#pragma once
#include "GameObject.h"

namespace cha
{
	/// @brief オブジェクトのインタフェース
	class IObject
	{
		friend class GameObjectCollection;
	public:
		IObject() = default;
		virtual ~IObject() = default;

		/// @brief インスタンス時に呼ばれる処理
		virtual void Start() = 0;
		/// @brief 毎フレームの更新処理
		virtual void Update() = 0;
		/// @brief 毎フレームの描画処理
		virtual void Draw() = 0;
		
		/// @brief 毎フレームのシャドウマップの描画処理
		virtual void DrawShadowMap() = 0;

		/// @brief ゲームオブジェクト終了時の処理
		virtual void OnDestroy() = 0;

		/// @brief ゲームオブジェクトへ変換
		/// @return ゲームオブジェクト
		virtual GameObject ToGameObject() const = 0;

		/// @brief ゲームオブジェクトIdを取得する
		/// @return ゲームオブジェクトId
		virtual const uint64_t GetId() const = 0;

		/// @brief ゲームオブジェクトがアクティブであるかを取得
		/// @return アクティブである true / false
		virtual const bool IsActive() const = 0;

		/// @brief ゲームオブジェクトをアクティブ化する
		/// @param _isActive アクティブ化 true / 非アクティブ化 false
		virtual void SetActive(const bool _isActive) = 0;

	protected:
		/// @brief ゲームオブジェクトIdを更新する
		/// @param _id 新しいId
		virtual void SetId(const uint64_t _id) = 0;
	};
}
