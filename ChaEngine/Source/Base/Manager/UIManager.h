#pragma once
#include "../Base.h"
#include "UIManager/UIRenderElement.h"

namespace cha
{
	/// @brief UIのマネージャー
	class UIManager : public IManager
	{
	public:
		/// @brief UIマネージャーの初期化処理
		UIManager() = default;
		/// @brief UIマネージャーの解放処理
		~UIManager() = default;
		
		/// @brief 更新処理としてUI描画をする
		/// @return 更新処理に成功 true / false
		bool Update();

		/// @brief 描画処理を追加する
		void AddRenderer(std::function<void()>&& _func, const UILayerOrder _order, const bool _isD2D = false);

	private:
		std::vector<UIRenderElement> renderQueue_{};  ///< 描画キュー
	};

}
