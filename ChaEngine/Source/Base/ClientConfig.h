#pragma once
#include "BasePch.h"

namespace cha
{
	/// @brief クライアントの設定構造体
	struct ClientConfig
	{
		Vec2Int clientSize{ 1600, 900 };         ///< クライアント領域の解像度
		Vec2 shadowMapSize{ 2048.0f, 2048.0f };  ///< シャドウマップの解像度
		uint32_t targetFPS{ 60 };                ///< 目標 FPS
		bool isWindow{ true };                   ///< ウィンドウモードか
	};
}
