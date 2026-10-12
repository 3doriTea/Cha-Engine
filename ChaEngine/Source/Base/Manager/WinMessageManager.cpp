
#include "WinMessageManager.h"
#include "../../resource.h"
#include <imgui/imgui_impl_dx11.h>

#include <vector>

#include <imm.h>
#pragma comment(lib, "imm32.lib")

// 変換中の文字列を保持するグローバル変数（描画用）
std::wstring g_imeCompText = L"";
// ゲーム側で管理する入力中テキストのバッファ（グローバル or クラスメンバー）
std::wstring g_inputText = L"";
bool g_isTextInputMode = false; // テキスト入力中かどうかのフラグ

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern LRESULT CursorRawInputHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

cha::WinMessageManager::WinMessageManager(const HINSTANCE _hInstance) :
	INSTANCE_HANDLE_{ _hInstance }
{
}

bool cha::WinMessageManager::Update()
{
	MSG msg{};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false; // アプリ終了
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}


void cha::WinMessageManager::BeginIME()
{
	g_isTextInputMode = true;
}

std::wstring_view cha::WinMessageManager::GetIME()
{
	return g_imeCompText;
}

LRESULT cha::WinMessageManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_isTextInputMode)
	{
		switch (message)
		{
		case WM_CHAR:
		{
			wchar_t ch = static_cast<wchar_t>(wParam);
			if (ch >= 32) {
				g_inputText.push_back(ch);
			}
			// Backspaceキーの処理なども必要であればここに書く
			if (ch == L'\b' && !g_inputText.empty()) {
				g_inputText.pop_back();
			}
			return 0; // ImGuiやデフォルトの処理に流さない
		}

		case WM_IME_COMPOSITION:
		{
			HIMC hIMC = ImmGetContext(hWnd);
			if (hIMC)
			{
				if (lParam & GCS_COMPSTR)
				{
					LONG size = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, nullptr, 0);
					if (size > 0)
					{
						std::vector<wchar_t> buffer(size / sizeof(wchar_t) + 1);
						ImmGetCompositionStringW(hIMC, GCS_COMPSTR, buffer.data(), size);
						g_imeCompText = buffer.data();
					}
					else
					{
						g_imeCompText = L"";
					}
				}

				if (lParam & GCS_RESULTSTR)
				{
					LONG size = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, nullptr, 0);
					if (size > 0)
					{
						std::vector<wchar_t> buffer(size / sizeof(wchar_t) + 1);
						ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, buffer.data(), size);

						g_inputText += buffer.data();
						g_imeCompText = L"";
					}
				}
				ImmReleaseContext(hWnd, hIMC);
			}
			//return 0; // ImGuiやデフォルトの処理に流さない
		}
		}
	}
	// --------------------------------------------------------------------------------

	// 通常時（テキスト入力中でない場合）は従来通りImGuiや生の入力を優先
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	if (LRESULT lResult = CursorRawInputHandler(hWnd, message, wParam, lParam))
	{
		return lResult;
	}

	switch (message)
	{
	case WM_COMMAND:
		// (省略)
		break;

	case WM_DESTROY:
		ClipCursor(nullptr);
		while (ShowCursor(TRUE) < 0);
		PostQuitMessage(0);
		break;

	case WM_SETFOCUS:
		if (g_isTextInputMode)
		{
			HIMC hIMC = ImmGetContext(hWnd);
			if (hIMC)
			{
				COMPOSITIONFORM cform{};
				cform.dwStyle = CFS_POINT;
				cform.ptCurrentPos.x = 100;
				cform.ptCurrentPos.y = 200;
				ImmSetCompositionWindow(hIMC, &cform);
				ImmReleaseContext(hWnd, hIMC);
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

//	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
//	{
//		return true;
//	}
//
//	if (LRESULT lResult = CursorRawInputHandler(hWnd, message, wParam, lParam))
//	{
//		return lResult;
//	}
//
//	switch (message)
//	{
//	case WM_COMMAND:
//	{
//		int wmId = LOWORD(wParam);
//		// 選択されたメニューの解析:
//		switch (wmId)
//		{
//		default:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//		}
//	}
//	break;
//	//case WM_PAINT:
//	//{
//	//	PAINTSTRUCT ps;
//	//	HDC hdc = BeginPaint(hWnd, &ps);
//	//	// TODO: HDC を使用する描画コードをここに追加してください...
//	//	EndPaint(hWnd, &ps); 
//	//break;
//	case WM_DESTROY:
//		// 終了時は確実にカーソルを戻す
//		ClipCursor(nullptr);
//		while (ShowCursor(TRUE) < 0);
//		PostQuitMessage(0);
//		break;
//
//#pragma region IME部分
//
//	case WM_SETFOCUS:
//		if (g_isTextInputMode)
//		{
//			// IMEの表示位置を入力カーソルの位置に合わせる設定
//			HIMC hIMC = ImmGetContext(hWnd);
//			if (hIMC)
//			{
//				COMPOSITIONFORM cform;
//				cform.dwStyle = CFS_POINT;
//				cform.ptCurrentPos.x = 100; // ゲーム内のテキストボックスのX座標
//				cform.ptCurrentPos.y = 200; // ゲーム内のテキストボックスのY座標
//				ImmSetCompositionWindow(hIMC, &cform);
//				ImmReleaseContext(hWnd, hIMC);
//			}
//		}
//		break;
//
//	case WM_IME_COMPOSITION:
//		if (g_isTextInputMode)
//		{
//			HIMC hIMC = ImmGetContext(hWnd);
//			if (hIMC)
//			{
//				// 変換中の文字列（未確定文字列）に変更があった場合
//				if (lParam & GCS_COMPSTR)
//				{
//					// 文字列の長さを取得
//					LONG size = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, nullptr, 0);
//					if (size > 0)
//					{
//						std::vector<wchar_t> buffer(size / sizeof(wchar_t) + 1);
//						ImmGetCompositionStringW(hIMC, GCS_COMPSTR, buffer.data(), size);
//						g_imeCompText = buffer.data(); // 変換中の文字列を更新
//					}
//					else
//					{
//						g_imeCompText = L"";
//					}
//				}
//
//				// 文字が確定された場合
//				if (lParam & GCS_RESULTSTR)
//				{
//					LONG size = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, nullptr, 0);
//					if (size > 0)
//					{
//						std::vector<wchar_t> buffer(size / sizeof(wchar_t) + 1);
//						ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, buffer.data(), size);
//
//						g_inputText += buffer.data(); // 確定文字列を本番バッファに追加
//						g_imeCompText = L"";         // 変換中バッファをクリア
//					}
//				}
//				ImmReleaseContext(hWnd, hIMC);
//			}
//			// DefWindowProcに流すとデフォルトの変な位置に文字が出ちゃうことがあるので防ぐ
//			return 0;
//		}
//		break;
//
//	case WM_CHAR:
//		// IME経由ではなく、直接英語や数字が打たれた場合は通常通り処理
//		if (g_isTextInputMode)
//		{
//			wchar_t ch = static_cast<wchar_t>(wParam);
//			if (ch >= 32) {
//				g_inputText.push_back(ch);
//			}
//			return 0;
//		}
//		break;
//
//#pragma endregion
//
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//	return 0;
}
