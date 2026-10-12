#pragma once
#include "../BasePch.h"
#include "IManager.h"

namespace cha
{
	class WinWindowManager;
	class Dx11Manager;

	class ImGuiManager : public IManager
	{
	public:
		ImGuiManager(WinWindowManager& _winWindow, Dx11Manager& _dx11Manager);
		~ImGuiManager();

		/// @brief 初期化処理
		/// @return 初期化に成功 true / false
		bool Initialize();

		/// @brief 新しいImGuiフレーム
		void NewFrame();

		/// @brief ImGuiの部分を手前に描画する
		void Render();

	private:
		WinWindowManager& winWindow_;
		Dx11Manager& dx11Manager_;
	};
}
