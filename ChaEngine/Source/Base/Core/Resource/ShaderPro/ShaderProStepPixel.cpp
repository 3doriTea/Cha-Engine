
#include "ShaderProStepPixel.h"
#include "../../../Manager/Dx11Manager.h"

cha::ShaderProStepPixel::ShaderProStepPixel(const RasterizerConfig& _rasterizerConfig) :
	rasterizerConfig_{ _rasterizerConfig }
{
}

bool cha::ShaderProStepPixel::TryLoad(
	Dx11Manager& _dx11,
	const fs::path& _file, ID3DInclude* _pInclude,
	const std::string_view _entryName,
	const std::string_view _version,
	const UINT _flags)
{
	HRESULT hResult{};
	ComPtr<ID3DBlob> pCompileError{};  // エラー格納用

	ComPtr<ID3DBlob> pCompilePS{};  // PS格納用

#pragma region ピクセルシェーダのコンパイル
	hResult = D3DCompileFromFile(
		_file.wstring().c_str(),
		nullptr,
		_pInclude,
		_entryName.data(),
		_version.data(),
		_flags,
		NULL,
		pCompilePS.GetAddressOf(),
		pCompileError.GetAddressOf());
	VCU_ASSERT(
		SUCCEEDED(hResult) && "ピクセルシェーダのコンパイルに失敗");
	if (FAILED(hResult))
	{
		return false;
	}
#pragma endregion

	bool succeed = _dx11.Ref(
		[&hResult, &pCompilePS, this](const ComPtr<ID3D11Device>& _pDevice) -> bool
	{
#pragma region ピクセルシェーダの作成
		hResult = _pDevice.Get()->CreatePixelShader(
			pCompilePS.Get()->GetBufferPointer(),
			pCompilePS.Get()->GetBufferSize(),
			nullptr,
			pPixelShader_.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "ピクセルシェーダの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion

#pragma region ラスタライザの作成
		const D3D11_RASTERIZER_DESC RASTERIZER_DESC
		{
			.FillMode = rasterizerConfig_.fillMode,
			.CullMode = rasterizerConfig_.cullMode,
			.FrontCounterClockwise = rasterizerConfig_.frontCounterClockwise,
		};

		hResult = _pDevice.Get()->CreateRasterizerState(
			&RASTERIZER_DESC,
			pRasterizerState_.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "ラスタライザステートの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion
		return true;
	});

	return succeed;
}

void cha::ShaderProStepPixel::Set(Dx11Manager& _dx11)
{
	_dx11.Ref([this](const ComPtr<ID3D11DeviceContext>& _pContext)
	{
		_pContext->PSSetShader(pPixelShader_.Get(), nullptr, 0);
		_pContext->RSSetState(pRasterizerState_.Get());
	});
}
