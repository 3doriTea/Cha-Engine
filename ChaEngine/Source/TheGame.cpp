// TheGame.cpp : アプリケーションのエントリ ポイントを定義します。

#include <Base/Base.h>
#include <Base/GameLoop.h>
#include "TheGame.h"

/// @brief ゲームのエントリポイント
void Main(const HINSTANCE _hInstance)
{
	cha::GameLoop gameLoop
	{{
	#if 0
		.clientSize = { 1920, 1080 },
		.targetFPS = 60,
		.isWindow = false,
	#elif 0
		.clientSize = { 1920, 1080 },
		.targetFPS = 60,
		.isWindow = true,
	#else
		.clientSize = { 1600, 900 },
		.targetFPS = 60,
		.isWindow = true,
	#endif
	}};

	gameLoop.Run(_hInstance);
}
