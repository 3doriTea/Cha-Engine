
#include "BlendStater.h"

bool cha::BlendStater::Initialize(const ComPtr<ID3D11Device>& _pDevice)
{
	// 書き込み先とのブレンド設定
	const D3D11_RENDER_TARGET_BLEND_DESC RENDER_TARGET_BLEND_DESC
	{
		.BlendEnable = TRUE,                                    // ブレンドの有無
		.SrcBlend = D3D11_BLEND_SRC_ALPHA,                      // 何に対して操作するか
		.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,                 // 現在の何に対して操作するか
		.BlendOp = D3D11_BLEND_OP_ADD,                          // 組み合わせる方法
		.SrcBlendAlpha = D3D11_BLEND_ONE,                       // なんのアルファに対して操作するか
		.DestBlendAlpha = D3D11_BLEND_ZERO,                     // 現在のなんのアルファに対して操作するか
		.BlendOpAlpha = D3D11_BLEND_OP_ADD,                     // アルファを組み合わせる方法
		.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,  // 書き込む先のマスク
	};

	// ブレンド設定
	const D3D11_BLEND_DESC BLEND_DESC
	{
		.AlphaToCoverageEnable = FALSE,
		.IndependentBlendEnable = FALSE,
		.RenderTarget = { RENDER_TARGET_BLEND_DESC },
	};

	// いざ作成
	HRESULT hResult{};
	hResult = _pDevice->CreateBlendState(&BLEND_DESC, pBlendState_[BlendStateType_Alpha].GetAddressOf());
	VCU_ASSERT(SUCCEEDED(hResult) && "コンスタントバッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;  // 初期化成功
}

void cha::BlendStater::Set(const BlendStateType _type, const ComPtr<ID3D11DeviceContext>& _pContext)
{
	// ブレンド混ぜ合わせ具合
	float blendFactor[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	
	switch (_type)
	{
	case BlendStateType_Alpha:  // 透過ok
		_pContext->OMSetBlendState(pBlendState_[_type].Get(), blendFactor, Color::WHITE);
		break;
	case BlendStateType_None:  // べた塗
		_pContext->OMSetBlendState(nullptr, nullptr, Color::WHITE);
		break;
	default:  // そのほかのblend
		VCU_ASSERT(false && "未実装のブレンドステート");
		break;
	}
}
