#pragma once
#include "../BasePch.h"
#include "IManager.h"

namespace cha
{
	/// @brief ウィンドウを扱うマネージャ
	class WinWindowManager : public IManager
	{
	public:
		/// @brief ウィンドウを扱うマネージャの初期化
		/// @param _clientSize クライアント領域のサイズ
		/// @param _isWindow ウィンドウモードであるか
		/// @param _hInstance インスタンスのハンドル
		/// @param _wndProc ウィンドウイベントのコールバック関数
		/// @param _title ウィンドウのタイトル
		/// @param _appName アプリケーションの名前
		WinWindowManager(
			const Vec2Int _clientSize,
			const bool _isWindow,
			const HINSTANCE _hInstance,
			const WNDPROC _wndProc,
			const std::wstring _title,
			const std::wstring _appName);

		/// @brief ウィンドウを表示する
		/// @param _nCmdShow 表示する際に渡すコマンドパラメータ
		/// @return 表示に成功 true / false
		bool Show(const int32_t _nCmdShow = 0);

		/// @brief ウィンドウハンドルを取得する
		/// @return ウィンドウハンドル (未表示の場合 nullptr)
		HWND GetWndHandle() const { return hWnd_; }

	private:
		const HINSTANCE INSTANCE_HANDLE_;  ///< インスタンスのハンドル
		const Vec2Int CLIENT_SIZE_;        ///< クライアント領域のサイズ
		const WNDPROC wndProc_;            ///< ウィンドウイベントコールバック関数
		const std::wstring TITLE_;         ///< タイトル文字列
		const std::wstring APP_NAME_;      ///< クラス名、アプリ名
		const bool IS_WINDOW_;             ///< ウィンドウモードであるか true / false
		HWND hWnd_{ nullptr };             ///< ウィンドウハンドル
	};
}
