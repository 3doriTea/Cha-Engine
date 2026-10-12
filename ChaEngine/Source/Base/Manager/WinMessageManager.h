#pragma once
#include "IManager.h"

namespace cha
{
	/// @brief Window メッセージ
	class WinMessageManager : public IManager
	{
	public:
		WinMessageManager(const HINSTANCE _hInstance);

		bool Update();

		/// @brief WndProc関数を取得
		/// @return WndProc関数ポインタ
		const WNDPROC GetWndProc() const { return WndProc; }

		void BeginIME();
		std::wstring_view GetIME();

	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		const HINSTANCE INSTANCE_HANDLE_;
	};
}
