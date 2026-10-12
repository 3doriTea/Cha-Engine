
#include "Shader.h"
#include "../../ManagerRegistry.h"
#include "../../Manager/Dx11Manager.h"

bool cha::ShaderRegistry::TryLoad(ShaderLoadConfig&& _config, ShaderResource& _out)
{
	HRESULT hResult{};

	// 頂点インプットレイアウト
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs{};

	ComPtr<ID3DBlob> pCompileError{};  // エラー格納用
	ComPtr<ID3DBlob> pCompileVS{};  // VS格納用
	ComPtr<ID3DBlob> pCompilePS{};  // PS格納用

	const ShaderCompileConfig& config{ compileConfig_ };

#pragma region 頂点シェーダのコンパイル
	hResult = D3DCompileFromFile(
		_config.file.wstring().c_str(),
		nullptr,
		config.pInclude,
		config.vertexEntryName.c_str(),
		config.vertexVersion.c_str(),
		config.flags,
		NULL,
		pCompileVS.GetAddressOf(),
		pCompileError.GetAddressOf());
	if (pCompileError)
	{
		std::string error = static_cast<const char*>(pCompileError.Get()->GetBufferPointer());
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

#pragma region ピクセルシェーダのコンパイル
	hResult = D3DCompileFromFile(
		_config.file.wstring().c_str(),
		nullptr,
		config.pInclude,
		config.pixelEntryName.c_str(),
		config.pixelVersion.c_str(),
		config.flags,
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
	hResult = pRefrection.Get()->GetDesc(&vsDesc);
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

	Dx11Manager& dx11 = ManagerRegistry::Ref<Dx11Manager>();

	bool succeed = dx11.Ref(
	[&hResult, &_out, &_config, pCompileVS, pCompilePS, &inputLayoutDescs](const ComPtr<ID3D11Device>& _pDevice) -> bool
	{
#pragma region 頂点シェーダの作成
		hResult = _pDevice.Get()->CreateVertexShader(
			pCompileVS.Get()->GetBufferPointer(),
			pCompileVS.Get()->GetBufferSize(),
			nullptr,
			_out.pVertexShader.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "頂点シェーダの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion

#pragma region ピクセルシェーダの作成
		hResult = _pDevice.Get()->CreatePixelShader(
			pCompilePS.Get()->GetBufferPointer(),
			pCompilePS.Get()->GetBufferSize(),
			nullptr,
			_out.pPixelShader.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "ピクセルシェーダの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion

#pragma region ラスタライザの作成
		const D3D11_RASTERIZER_DESC RASTERIZER_DESC
		{
			.FillMode = _config.rasterizerConfig.fillMode,
			.CullMode = _config.rasterizerConfig.cullMode,
			.FrontCounterClockwise = _config.rasterizerConfig.frontCounterClockwise,
		};

		hResult = _pDevice.Get()->CreateRasterizerState(
			&RASTERIZER_DESC,
			_out.pRasterizerState.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "ラスタライザステートの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion

#pragma region 頂点インプットレイアウトの作成
		hResult = _pDevice.Get()->CreateInputLayout(
			inputLayoutDescs.data(),
			static_cast<UINT>(inputLayoutDescs.size()),
			pCompileVS.Get()->GetBufferPointer(),
			pCompileVS.Get()->GetBufferSize(),
			_out.pInputLayout.GetAddressOf());
		VCU_ASSERT(SUCCEEDED(hResult) && "頂点インプットレイアウトの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}
#pragma endregion
		return true;
	});

	return true;  // ここまで来たら読み込み成功
}
