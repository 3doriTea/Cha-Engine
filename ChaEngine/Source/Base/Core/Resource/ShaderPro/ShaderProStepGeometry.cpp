
#include "ShaderProStepGeometry.h"
#include "../../../Manager/Dx11Manager.h"

bool cha::ShaderProStepGeometry::TryLoad(
	Dx11Manager& _dx11,
	const fs::path& _file,
	ID3DInclude* _pInclude,
	const std::string_view _entryName,
	const std::string_view _version,
	const UINT _flags)
{
	HRESULT hResult{};
	ComPtr<ID3DBlob> pCompileError{};  // エラー格納用

	ComPtr<ID3DBlob> pCompileGS{};  // GS格納用

#pragma region ピクセルシェーダのコンパイル
	hResult = D3DCompileFromFile(
		_file.wstring().c_str(),
		nullptr,
		_pInclude,
		_entryName.data(),
		_version.data(),
		_flags,
		NULL,
		pCompileGS.GetAddressOf(),
		pCompileError.GetAddressOf());
	VCU_ASSERT(
		SUCCEEDED(hResult) && "ジオメトリシェーダのコンパイルに失敗");
	if (FAILED(hResult))
	{
		return false;
	}
#pragma endregion

	bool succeed = _dx11.Ref(
		[&hResult, &pCompileGS, this](const ComPtr<ID3D11Device>& _pDevice) -> bool
	{
#pragma region ジオメトリシェーダの作成
		hResult = _pDevice.Get()->CreateGeometryShader(
			pCompileGS.Get()->GetBufferPointer(),
			pCompileGS.Get()->GetBufferSize(),
			nullptr,
			pGeometryShader_.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "ジオメトリシェーダの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion

		return true;
	});

	return succeed;
}

void cha::ShaderProStepGeometry::Set(Dx11Manager& _dx11)
{
	_dx11.Ref([this](const ComPtr<ID3D11DeviceContext>& _pContext)
	{
		_pContext->GSSetShader(pGeometryShader_.Get(), nullptr, 0); // GSをセット
	});
}
