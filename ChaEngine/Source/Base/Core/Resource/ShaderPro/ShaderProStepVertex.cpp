
#include "ShaderProStepVertex.h"
#include "../../../Manager/Dx11Manager.h"

bool cha::ShaderProStepVertex::TryLoad(
	Dx11Manager& _dx11,
	const fs::path& _file,
	ID3DInclude* _pInclude,
	const std::string_view _entryName,
	const std::string_view _version,
	const UINT _flags)
{
	HRESULT hResult{};
	ComPtr<ID3DBlob> pCompileError{};  // エラー格納用

	ComPtr<ID3DBlob> pCompileVS{};  // VS格納用
	// 頂点インプットレイアウト
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs{};

#pragma region 頂点シェーダのコンパイル
	hResult = D3DCompileFromFile(
		_file.wstring().c_str(),
		nullptr,
		_pInclude,
		_entryName.data(),
		_version.data(),
		_flags,
		NULL,
		pCompileVS.GetAddressOf(),
		pCompileError.GetAddressOf());
	if (pCompileError)
	{
		std::string error = static_cast<const char*>(pCompileError->GetBufferPointer());
		VCU_ASSERT(
			SUCCEEDED(hResult) && "頂点シェーダのコンパイルに失敗");
	}
	else
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "頂点シェーダのコンパイルに失敗");
	}

	if (FAILED(hResult))
	{
		return false;
	}
#pragma endregion

#pragma region 頂点インプットレイアウトの抽出
	ComPtr<ID3D11ShaderReflection> pRefrection{};
	hResult = D3DReflect(
		pCompileVS->GetBufferPointer(),
		pCompileVS->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(pRefrection.GetAddressOf()));
	VCU_ASSERT(SUCCEEDED(hResult) && "頂点シェーダのリフレクションに失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	D3D11_SHADER_DESC vsDesc{};
	hResult = pRefrection->GetDesc(&vsDesc);
	VCU_ASSERT(SUCCEEDED(hResult) && "頂点シェーダのDESC取得に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	for (UINT i = 0; i < vsDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc{};
		pRefrection->GetInputParameterDesc(i, &paramDesc);
		VCU_ASSERT(
			SUCCEEDED(hResult) && "頂点シェーダのDESC取得に失敗");
		if (FAILED(hResult))
		{
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC desc{};
		desc.SemanticName = paramDesc.SemanticName;
		desc.SemanticIndex = paramDesc.SemanticIndex;
		desc.InputSlot = 0;
		desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			desc.InstanceDataStepRate = 0;

		// フォーマットをビットマスクから判定していくー
		switch (paramDesc.Mask)
		{
		case 0b1:
			switch (paramDesc.ComponentType)
			{
			case D3D_REGISTER_COMPONENT_UINT32:
				desc.Format = DXGI_FORMAT_R32_UINT;
				break;
			case D3D_REGISTER_COMPONENT_SINT32:
				desc.Format = DXGI_FORMAT_R32_SINT;
				break;
			case D3D_REGISTER_COMPONENT_FLOAT32:
				desc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			default:
				VCU_ASSERT(
					false && "知らないコンポーネントフォーマット");
				return false;
			}
			break;
		case 0b11:
			desc.Format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case 0b111:
			desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case 0b1111:
			switch (paramDesc.ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32:   desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;   break; // これで BONE_INDICES が正しく UINT になる
				case D3D_REGISTER_COMPONENT_SINT32:   desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;   break;
				case D3D_REGISTER_COMPONENT_FLOAT32:  desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  break; // BONE_WEIGHTS はここを通る
				default: VCU_ASSERT(false && "知らないコンポーネントフォーマット"); return false;
			}
			break;
		default:
			VCU_ASSERT(
				false && "未対応のマスク");
			return false;
		}

		inputLayoutDescs.push_back(desc);
	}
#pragma endregion

#pragma region 頂点シェーダの作成
	bool succeed = _dx11.Ref(
		[&hResult, &pCompileVS, &inputLayoutDescs, this](const ComPtr<ID3D11Device>& _pDevice) -> bool
	{
		hResult = _pDevice->CreateVertexShader(
			pCompileVS->GetBufferPointer(),
			pCompileVS->GetBufferSize(),
			nullptr,
			pVertexShader_.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "頂点シェーダの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}

#pragma region 頂点インプットレイアウトの作成
		hResult = _pDevice->CreateInputLayout(
			inputLayoutDescs.data(),
			static_cast<UINT>(inputLayoutDescs.size()),
			pCompileVS->GetBufferPointer(),
			pCompileVS->GetBufferSize(),
			pInputLayout_.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "頂点インプットレイアウトの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion

		return true;
	});
#pragma endregion

	return succeed;
}

void cha::ShaderProStepVertex::Set(Dx11Manager& _dx11)
{
	_dx11.Ref([this](const ComPtr<ID3D11DeviceContext>& _pContext)
	{
		_pContext->VSSetShader(pVertexShader_.Get(), nullptr, 0);
		_pContext->IASetInputLayout(pInputLayout_.Get());
	});
}
