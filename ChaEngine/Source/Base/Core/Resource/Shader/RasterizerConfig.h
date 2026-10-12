#pragma once
#include "../../../BasePch.h"

namespace cha
{
	/// @brief ラスタライザの設定
	struct RasterizerConfig
	{
		D3D11_CULL_MODE cullMode  { D3D11_CULL_BACK };   // 面消去モード(カリング)
		D3D11_FILL_MODE fillMode  { D3D11_FILL_SOLID };  // 塗り潰しモード
		bool frontCounterClockwise{ true };              // 面の表 == 頂点の反時計回り か
	};
}
