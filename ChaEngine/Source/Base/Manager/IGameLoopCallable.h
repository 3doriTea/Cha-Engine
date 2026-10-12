#pragma once

namespace cha
{
	/// @brief ゲームループから呼び出し可能なインタフェース
	class IGameLoopCallable
	{
		friend class GameLoop;
	public:
		IGameLoopCallable() = default;
		virtual ~IGameLoopCallable() = default;

	private:
		/// @brief 更新処理
		/// @return 更新された true / false
		virtual bool Update() = 0;

		/// @brief 遅い更新処理
		/// @return 更新された true / false
		virtual bool LateUpdate() = 0;

		/// @brief 描画処理
		/// @param _isShadowing 影の描画を行うか
		/// @return 描画された true / false
		virtual bool Draw(const bool _isShadowing) = 0;

		/// @brief ゲームシーンのお掃除をする
		/// @return お掃除された true / false
		virtual bool Refresh() = 0;
	};
}
