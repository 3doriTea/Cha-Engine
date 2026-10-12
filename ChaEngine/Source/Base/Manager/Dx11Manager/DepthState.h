#pragma once
#include "../../BasePch.h"

namespace cha
{
	struct DepthState
	{
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_{};    // 深度バッファビュー
		ComPtr<ID3D11DepthStencilState> pDepthStencilState_{};  // 深度バッファステート
	};
}
