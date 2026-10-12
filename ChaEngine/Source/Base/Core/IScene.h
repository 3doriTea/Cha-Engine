#pragma once

namespace cha
{
	class GameObjectCollection;

	/// @brief シーンのインタフェース
	class IScene
	{
	public:
		IScene() = default;
		virtual ~IScene() = default;

		/// @brief シーン開始時の処理
		virtual void Start() = 0;

		/// @brief シーンの更新処理
		virtual void Update() = 0;

		/// @brief シーンの遅い更新処理
		virtual void LateUpdate() = 0;

		/// @brief シーンの描画処理
		virtual void Draw(const bool _isShadowing) = 0;

		/// @brief シーン終了時の処理
		virtual void End() = 0;

		/// @brief ゲームオブジェクトコレクションを参照
		/// @return 参照ポインタ
		virtual GameObjectCollection& RefGameObjectCollection() = 0;
	};
}
