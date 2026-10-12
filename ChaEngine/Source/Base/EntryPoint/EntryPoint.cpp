#include "EntryPoint.h"

/// @brief 本当のWindowsアプリケーションエントリポイント
/// @param hInstance プログラムのインスタンスハンドル
/// @param hPrevInstance 常に0
/// @param lpCmdLine コマンドライン引数(Unicode)
/// @param nCmdShow 起動時のオプション
/// @return 別のプログラムへ渡す用の戻り値
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// 使わない引数たち
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ゲーム側のエントリポイントを呼び出す
	Main(hInstance);

	return 0;
}
