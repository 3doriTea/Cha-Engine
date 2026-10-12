#pragma once
#include "ClientConfig.h"

namespace cha
{
	/// @brief ゲームループを扱う
	class GameLoop
	{
	public:
		GameLoop(const ClientConfig _clientConfig);

		/// @brief 実行
		/// @param _hInstance アプリケーションのインスタンスポインタ
		void Run(const HINSTANCE _hInstance);

	private:
		ClientConfig clientConfig_;  // クライアント設定
	};
}
