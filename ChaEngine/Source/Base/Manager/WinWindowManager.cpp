
#include "WinWindowManager.h"
#include "../resource.h"

cha::WinWindowManager::WinWindowManager(
	const Vec2Int _clientSize,
	const bool _isWindow,
	const HINSTANCE _hInstance,
	const WNDPROC _wndProc,
	const std::wstring _title,
	const std::wstring _appName) :
	CLIENT_SIZE_{ _clientSize },
	INSTANCE_HANDLE_{ _hInstance },
	IS_WINDOW_{ _isWindow },
	wndProc_{ _wndProc },
	TITLE_{ _title },
	APP_NAME_{ _appName }
{
}

bool cha::WinWindowManager::Show(const int32_t _nCmdShow)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	const WNDCLASSEXW WNDCLASSEXW_SETTING
	{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = wndProc_,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = INSTANCE_HANDLE_,
		.hIcon = LoadIcon(INSTANCE_HANDLE_, MAKEINTRESOURCE(IDI_ICON1)),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = nullptr, //MAKEINTRESOURCEW(IDC_DIESHACLIENT),
		.lpszClassName = APP_NAME_.c_str(),
		.hIconSm = LoadIcon(INSTANCE_HANDLE_, MAKEINTRESOURCE(IDI_ICON1)),
	};
	
	ATOM atom = RegisterClassExW(&WNDCLASSEXW_SETTING);
	if (atom == 0)
	{
		VCU_ASSERT(atom != 0);
		return false;
	}

	DWORD style = IS_WINDOW_
		? (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX) | WS_VISIBLE
		: (WS_POPUP | WS_VISIBLE);

	DWORD dwExStyle = WS_EX_APPWINDOW;

	// Clientサイズ を考慮した Windowサイズ を求める
	RECT windowRect{ 0, 0, CLIENT_SIZE_.x, CLIENT_SIZE_.y };
	if (IS_WINDOW_)
	{
		BOOL succeed{ AdjustWindowRectEx(&windowRect, style, FALSE, dwExStyle) };
		
		if (!succeed)
		{
			VCU_ASSERT(succeed && "クライアント領域を考慮したウィンドウサイズ計算に失敗");
			return false;
		}
	}

	const Vec2Int WINDOW_SIZE
	{
		// 計算されたウィンドウのサイズ 横幅
		windowRect.right - windowRect.left,
		// 計算されたウィンドウのサイズ 高さ
		windowRect.bottom - windowRect.top,
	};

	hWnd_ = CreateWindowExW(
		dwExStyle,
		APP_NAME_.c_str(),
		TITLE_.c_str(),
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_SIZE.x,
		WINDOW_SIZE.y,
		nullptr,  // 親ウィンドウハンドル
		nullptr,  // メニューハンドル
		INSTANCE_HANDLE_,
		nullptr);

	if (!hWnd_)
	{
		VCU_ASSERT(hWnd_ && "ウィンドウ作成に失敗");
		return false;
	}

	if (!IsWindow(hWnd_))
	{
		VCU_ASSERT(IsWindow(hWnd_) && "正しいウィンドウ作成に失敗");
		return false;
	}

	return true;
}
