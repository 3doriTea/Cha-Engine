
#include "Dx11Manager.h"
#include "../Core/Resource.h"

#include "../Core/Resource/Surface.h"
#include "../Core/Resource/Material.h"
#include "../Core/Resource/Shader.h"
#include "../Core/Resource/Audio.h"
#include "../Core/Resource/Texture.h"
#include "../Core/Resource/ShaderPro.h"

#include "../../Game/System/CameraSystem/CameraMaterialProperty.h"

// simple transparent queue
static std::vector<std::function<void()>> g_transparentQueue;

cha::Dx11Manager::Dx11Manager(
	const HWND _hWnd,
	const Vec2Int _clientSize,
	const Vec2 _shadowMapSize,
	const bool _isWindow) :
	WND_HANDLE_{ _hWnd },
	CLIENT_SIZE_{ _clientSize },
	SHADOW_MAP_SIZE_{ _shadowMapSize },
	IS_WINDOW_{ _isWindow }
{
}

void cha::Dx11Manager::EnqueueTransparent(std::function<void()> _fn)
{
	g_transparentQueue.push_back(std::move(_fn));
}

void cha::Dx11Manager::EnqueueTransparentFront(std::function<void()> _fn)
{
	g_transparentQueue.insert(g_transparentQueue.begin(), std::move(_fn));
}

void cha::Dx11Manager::FlushTransparentQueue()
{
	for (auto& fn : g_transparentQueue)
	{
		fn();
	}
	g_transparentQueue.clear();
}

bool cha::Dx11Manager::Initialize()
{
	HRESULT hResult{};

	ComPtr<IDXGIFactory> factory{};

	hResult = CreateDXGIFactory1(__uuidof(factory), &factory);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "DXGIFactoryの作成に失敗");
		return false;
	}

	UINT flags{ 0 };

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// d2d用にシングルスレッド明記
	flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	// d2d用に色サポートも追加
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL futureLevel{};
	hResult = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		pDevice_.GetAddressOf(),
		&futureLevel,
		pContext_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "DeviceとDeviceContextの作成に失敗");
		return false;
	}

	ResetViewport(CLIENT_SIZE_);
	bool succeed{ false };

#pragma region 深度バッファを作る
	// 震度バッファを作成
	succeed = GenerateDepthBuffer(CLIENT_SIZE_);
	VCU_ASSERT(succeed && "深度バッファの作成に失敗");

	// 普通の深度ステート
	currentDepthStateType_ = DepthStateType_Standard;
	succeed = GenerateDepthState(
		{
			.DepthEnable = TRUE,
			.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D11_COMPARISON_LESS,
			.StencilEnable = {},
			.StencilReadMask = {},
			.StencilWriteMask = {},
			.FrontFace = {},
			.BackFace = {},
		});
	VCU_ASSERT(succeed && "深度ステート(スタンダード)の作成に失敗");

	// 最背面描画用の深度ステート
	currentDepthStateType_ = DepthStateType_Background;
	succeed = GenerateDepthState(
		{
			.DepthEnable = TRUE,
			.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
			.DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
			.StencilEnable = {},
			.StencilReadMask = {},
			.StencilWriteMask = {},
			.FrontFace = {},
			.BackFace = {},
		});
	VCU_ASSERT(succeed && "深度ステート(スタンダード)の作成に失敗");

	currentDepthStateType_ = DepthStateType_None;
	succeed = GenerateDepthState(
		{
			.DepthEnable = FALSE,                        // 深度テスト自体を無効化
			.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO, // 深度バッファに書き込まない
			.DepthFunc = D3D11_COMPARISON_ALWAYS,         // 常に合格
			.StencilEnable = {},
			.StencilReadMask = {},
			.StencilWriteMask = {},
			.FrontFace = {},
			.BackFace = {},
		});
	VCU_ASSERT(succeed && "深度ステート(None)の作成に失敗");

	// デフォルトでスタンダード描画
	ChangeDepthState(DepthStateType_Standard);
#pragma endregion

	succeed = blendStater.Initialize(pDevice_);
	VCU_ASSERT(succeed && "深度ステート(None)の作成に失敗");
	if (!succeed)
	{
		return false;
	}

	// シャドウマップの作成
	GenerateShadowMap(
		static_cast<UINT>(SHADOW_MAP_SIZE_.x),
		static_cast<UINT>(SHADOW_MAP_SIZE_.y));

	return GenerateSwapChain(factory);
}

void cha::Dx11Manager::Clear()
{
	pContext_->ClearRenderTargetView(
		pBackBufferRTV_.Get(),
		fillColor_.f);

	if (depthState_[DepthStateType_Standard].pDepthStencilView_)
	{
		pContext_->ClearDepthStencilView(
			depthState_[DepthStateType_Standard].pDepthStencilView_.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,  // 最大深度でクリア
			0      // ステンシルクリア値
		);
	}
}

void cha::Dx11Manager::Swap()
{
	pSwapChain_->Present(0, 0);
}

void cha::Dx11Manager::ClearAndBindShadowMap()
{
	// シャドウマップをクリアする
	pContext_->ClearDepthStencilView(pShadowDSV_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// 描画先のレンダーターゲットはなし、深度ステンシルビューだけ指定
	ID3D11RenderTargetView* nullRTV{ nullptr };
	pContext_->OMSetRenderTargets(1, &nullRTV, pShadowDSV_.Get());

	// シャドウマップ用のビューポート
	D3D11_VIEWPORT shadowViewport
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = SHADOW_MAP_SIZE_.x,  // テクスチャ作成時の幅
		.Height = SHADOW_MAP_SIZE_.y, // テクスチャ作成時の高さ
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f
	};
	// ビューポートをセット
	pContext_->RSSetViewports(1, &shadowViewport);
}

void cha::Dx11Manager::UnbindShadowMap()
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	pContext_->PSSetShaderResources(10, 1, nullSRV);

	ID3D11RenderTargetView* nullRTV[1] = { nullptr };
	pContext_->OMSetRenderTargets(1, nullRTV, nullptr);

	ResetViewport(CLIENT_SIZE_);
	ResetRenderTargets();
}

void cha::Dx11Manager::BindShadowMap()
{
	SetTexture(*pShadowTexture_.get(), 10);
}

void cha::Dx11Manager::SetSurface(
	const Resource<ResourceType::Surface>& _resource)
{
	SetSurface(_resource.Get());
}

void cha::Dx11Manager::SetSurface(const SurfaceResource& surface)
{
	UINT stride{ surface.size };
	UINT offset{ surface.offset };
	pContext_->IASetVertexBuffers(0, 1, surface.pVertexBuffer.GetAddressOf(), &stride, &offset);
	
	// もしインデックスバッファが指定されていればセットする
	if (surface.pIndexBuffer)
	{
		pContext_->IASetIndexBuffer(surface.pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
	else  // 指定されていなければ未指定
	{
		pContext_->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	}
	pContext_->IASetPrimitiveTopology(surface.toplogy);

	drawIndexCount_ = surface.indexCount;
	drawVertexCount_ = surface.vertexCount;

#if 0  // フルで描画ログを出す
	VCU_LOG("SetSurface: pVertexBuffer_={} stride={} offset={} topology={} vertexCount={} indexCount={}",
		(void*)(surface.pVertexBuffer.Get()),
		static_cast<int>(stride),
		static_cast<int>(offset),
		static_cast<int>(surface.toplogy),
		static_cast<int>(drawVertexCount_),
		static_cast<int>(drawIndexCount_));
#endif
}

void cha::Dx11Manager::SetShader(const Resource<ResourceType::Shader>& _resource)
{
	pContext_->VSSetShader(_resource.Get().pVertexShader.Get(), nullptr, 0);
	pContext_->IASetInputLayout(_resource.Get().pInputLayout.Get());
	pContext_->PSSetShader(_resource.Get().pPixelShader.Get(), nullptr, 0);
	pContext_->RSSetState(_resource.Get().pRasterizerState.Get());
	pContext_->GSSetShader(nullptr, nullptr, 0);
}

void cha::Dx11Manager::SetShader(const Resource<ResourceType::ShaderPro>& _resource)
{
	// 一旦すべてのシェーダを取り外す
	pContext_->VSSetShader(nullptr, nullptr, 0);
	pContext_->IASetInputLayout(nullptr);
	pContext_->PSSetShader(nullptr, nullptr, 0);
	pContext_->RSSetState(nullptr);
	pContext_->GSSetShader(nullptr, nullptr, 0);

	//VCU_LOG("SetShader(ShaderProResource): steps count={}", static_cast<int>(_resource.Get().steps.size()));
	//// 自身を参照させセットする
	//for (size_t i = 0; i < _resource.Get().steps.size(); ++i)
	//{
	//	VCU_LOG("SetShader(ShaderProResource): calling step {}", static_cast<int>(i));
	//	_resource.Get().steps[i]->Set(*this);
	//}

	for (auto& pStep : _resource.Get().steps)
	{
		pStep->Set(*this);
	}
}

void cha::Dx11Manager::SetTexture(const Resource<ResourceType::Texture>& _resource, const UINT _slot)
{
	auto& test = _resource.Get();

	pContext_->PSSetShaderResources(_slot, 1, _resource.Get().pSRV.GetAddressOf());
	pContext_->PSSetSamplers(_slot, 1, _resource.Get().pSamplerState.GetAddressOf());
}

void cha::Dx11Manager::SetMaterial(const Resource<ResourceType::Material>& _resource)
{
	const MaterialResource& material = _resource.Get();

	// シェーダの指定があるならセット
	if (!material.shader.IsNull())
	{
		SetShader(material.shader);
	}

	// テクスチャがあるならセット
	if (!material.texture.IsNull())
	{
		SetTexture(material.texture);
	}

	// プロパティを送信する
	if (material.pProperty)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		HRESULT hResult = pContext_->Map(
			material.pConstantBuffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,  // 破棄する
			//D3D11_MAP_WRITE_NO_OVERWRITE,
			0,
			&mappedResource);

		if (FAILED(hResult))
		{
			VCU_LOG("MAP失敗");
			return;  // 失敗
		}

		memcpy(mappedResource.pData, material.pProperty->GetPtr(), material.pProperty->GetSize());
		pContext_->Unmap(material.pConstantBuffer.Get(), 0);

		pContext_->VSSetConstantBuffers(material.slot, 1, material.pConstantBuffer.GetAddressOf());
		pContext_->PSSetConstantBuffers(material.slot, 1, material.pConstantBuffer.GetAddressOf());
		// Also bind to Geometry Shader stage so GS can access camera/projection constants
		pContext_->GSSetConstantBuffers(material.slot, 1, material.pConstantBuffer.GetAddressOf());
		// Ensure PS slot 2 is populated so Trail::Draw can safely bind to slot 2.
		// Bind the same constant buffer to slot 2 as a fallback (no-op if material.slot == 2).
		pContext_->PSSetConstantBuffers(2, 1, material.pConstantBuffer.GetAddressOf());
	}

	// TODO: 一時的に0
	if (0) {
		// 1. 本番バッファと同じサイズのステージングバッファを用意
		D3D11_BUFFER_DESC desc = {};
		material.pConstantBuffer->GetDesc(&desc);
		desc.Usage = D3D11_USAGE_STAGING;      // CPU読み取り用
		desc.BindFlags = 0;                    // パイプラインにはバインドしない
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;

		ComPtr<ID3D11Buffer> pStagingBuffer{};
		pDevice_->CreateBuffer(&desc, nullptr, &pStagingBuffer);

		// 2. 本番(GPU側)からステージング(CPU側)へコピー
		pContext_->CopyResource(pStagingBuffer.Get(), material.pConstantBuffer.Get());

		// 3. Mapして中身を覗く
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(pContext_->Map(pStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			// 送信した構造体でキャストして読み取る
			//CameraMaterialProperty* data = static_cast<CameraMaterialProperty*>(mappedResource.pData);
			DirectX::XMMATRIX* rowData{ static_cast<DirectX::XMMATRIX*>(mappedResource.pData) };
			// data->toWorldMatrix などを確認

			pContext_->Unmap(pStagingBuffer.Get(), 0);

			UINT slotNumber{ material.slot };

			int a{};
		}
	}
}

void cha::Dx11Manager::Draw() const
{
	pContext_->DrawIndexed(drawIndexCount_, 0, 0);
}

void cha::Dx11Manager::Draw(const UINT _indexCount) const
{
	pContext_->DrawIndexed(_indexCount, 0, 0);
}

void cha::Dx11Manager::DrawVertex(const UINT _vertexCount) const
{
	// Diagnostic: query current IA and shader bindings to help debug pipeline state
	D3D11_PRIMITIVE_TOPOLOGY currentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	pContext_->IAGetPrimitiveTopology(&currentTopology);

	ComPtr<ID3D11Buffer> pVB{};
	UINT stride = 0;
	UINT offset = 0;
	pContext_->IAGetVertexBuffers(0, 1, pVB.GetAddressOf(), &stride, &offset);

	ComPtr<ID3D11VertexShader> pVS{};
	ComPtr<ID3D11PixelShader> pPS{};
	ComPtr<ID3D11GeometryShader> pGS{};
	pContext_->VSGetShader(pVS.GetAddressOf(), nullptr, nullptr);
	pContext_->PSGetShader(pPS.GetAddressOf(), nullptr, nullptr);
	pContext_->GSGetShader(pGS.GetAddressOf(), nullptr, nullptr);

#if 0  // すべての描画ログ
	VCU_LOG("DrawVertex: _vertexCount={} currentTopology={} pVB={} stride={} offset={} pVS={} pPS={} pGS={}",
		static_cast<int>(_vertexCount),
		static_cast<int>(currentTopology),
		(void*)pVB.Get(),
		static_cast<int>(stride),
		static_cast<int>(offset),
		(void*)pVS.Get(),
		(void*)pPS.Get(),
		(void*)pGS.Get());
#endif

	pContext_->Draw(_vertexCount, 0);
}

bool cha::Dx11Manager::GenerateSwapChain(ComPtr<IDXGIFactory>& _factory)
{
	DXGI_SWAP_CHAIN_DESC desc
	{
		.BufferDesc
		{
			.Width = static_cast<UINT>(CLIENT_SIZE_.x),
			.Height = static_cast<UINT>(CLIENT_SIZE_.y),
			.RefreshRate
			{
				.Numerator = 0,
				.Denominator = 1,
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		},
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = WND_HANDLE_,
		.Windowed = IS_WINDOW_,  // ウィンドウモード
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
	};

	HRESULT hResult = _factory->CreateSwapChain(
		pDevice_.Get(),
		&desc,
		pSwapChain_.GetAddressOf());

	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "スワップチェーンの作成に失敗");
		return false;
	}

	return GenerateBackBufferRTV(desc);
}

bool cha::Dx11Manager::GenerateBackBufferRTV(const DXGI_SWAP_CHAIN_DESC& _swapChainDesc)
{
	const D3D11_RENDER_TARGET_VIEW_DESC DESC
	{
		.Format = _swapChainDesc.BufferDesc.Format,
		.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
		.Buffer{},
	};

	ComPtr<ID3D11Texture2D> pBackBuffer{};

	HRESULT hResult{};
	hResult = pSwapChain_->GetBuffer(0, __uuidof(pBackBuffer), &pBackBuffer);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "スワップチェーンのバッファ取得に失敗");
		return false;
	}

	hResult = pDevice_->CreateRenderTargetView(
		pBackBuffer.Get(),
		&DESC,
		pBackBufferRTV_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "スワップチェーンのバッファRTV作成に失敗");
		return false;
	}

	return ResetRenderTargets();
}

bool cha::Dx11Manager::ResetViewport(const Vec2 _size)
{
	if (_size.x <= 0.0f || _size.y <= 0.0f)
	{
		return false;
	}

	currentViewport_ = D3D11_VIEWPORT
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = _size.x,
		.Height = _size.y,
		.MinDepth = 0.0,
		.MaxDepth = 1.0,
	};

	pContext_->RSSetViewports(1, &currentViewport_);

	return true;
}

bool cha::Dx11Manager::ResetRenderTargets()
{
	// レンダーターゲットを再セット
	pContext_->OMSetRenderTargets(
		1,
		pBackBufferRTV_.GetAddressOf(),
		depthState_[currentDepthStateType_].pDepthStencilView_.Get());
	return true;
}

void cha::Dx11Manager::SetBlendState(const BlendStateType _type)
{
	// ブレンドステーターにコンテキストとともに渡す
	blendStater.Set(_type, pContext_);
}

bool cha::Dx11Manager::GenerateDepthState(const D3D11_DEPTH_STENCIL_DESC& _desc)
{
	HRESULT hResult{};

	hResult = pDevice_->CreateDepthStencilView(
		pDepthStencilBuffer_.Get(),
		nullptr,
		depthState_[currentDepthStateType_].pDepthStencilView_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "深度バッファビューの作成に失敗");
		return false;
	}

	{
		hResult = pDevice_->CreateDepthStencilState(&_desc, depthState_[currentDepthStateType_].pDepthStencilState_.GetAddressOf());
		if (FAILED(hResult))
		{
			VCU_ASSERT(SUCCEEDED(hResult) && "深度バッファステートの作成に失敗");
			return false;
		}

		// セットしておく
		pContext_->OMSetDepthStencilState(depthState_[currentDepthStateType_].pDepthStencilState_.Get(), 1);
	}

	return true;
}

bool cha::Dx11Manager::GenerateDepthBuffer(const Vec2 _size)
{
	HRESULT hResult{};

	// 深度バッファ作成
	const D3D11_TEXTURE2D_DESC DESC
	{
		.Width = static_cast<UINT>(_size.x),
		.Height = static_cast<UINT>(_size.y),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,  // 深度フォーマット
		.SampleDesc =
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	hResult = pDevice_->CreateTexture2D(&DESC, nullptr, pDepthStencilBuffer_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "深度バッファの作成に失敗");
		return false;
	}

	return true;
}

bool cha::Dx11Manager::GenerateShadowMap(const UINT _shadowMapWidth, const UINT _shadowMapHeight)
{
	HRESULT hResult{};

	{  // テクスチャ作る
		const D3D11_TEXTURE2D_DESC DESC = D3D11_TEXTURE2D_DESC
		{
			.Width = _shadowMapWidth,
			.Height = _shadowMapHeight,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT_R24G8_TYPELESS,  // 型未指定の24bit深度 + 8bit
			.SampleDesc =
			{
				.Count = 1,
				.Quality = 0,
			},
			.Usage = D3D11_USAGE_DEFAULT,
			// 深度バッファとしてもシェーダリソースとしてもどっちも使う
			.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
			.CPUAccessFlags = 0,
			.MiscFlags = 0,
		};

		hResult = pDevice_->CreateTexture2D(&DESC, nullptr, pShadowTexture2D_.GetAddressOf());
		if (FAILED(hResult))
		{
			VCU_ASSERT(SUCCEEDED(hResult) && "シャドウマップ深度バッファの作成に失敗");
			return false;
		}
	}

	{  // テクスチャを作る
		pShadowTexture_ = std::make_unique<Resource<ResourceType::Texture>>(
			TextureType_ShadowMap,
			pShadowTexture2D_
		);
	}

	{  // 深度ステンシルビューを作る
		const D3D11_DEPTH_STENCIL_VIEW_DESC DESC
		{
			.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,  // 深度型は24D
			.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
			.Flags = {},
			.Texture2D
			{
				.MipSlice = 0
		}
		};

		hResult = pDevice_->CreateDepthStencilView(pShadowTexture2D_.Get(), &DESC, pShadowDSV_.GetAddressOf());
		if (FAILED(hResult))
		{
			VCU_ASSERT(SUCCEEDED(hResult) && "シャドウマップ深度バッファビューの作成に失敗");
			return false;
		}
	}

	return true;
}


void cha::Dx11Manager::ChangeDepthState(const DepthStateType _depthStateType)
{
	currentDepthStateType_ = _depthStateType;
	ResetRenderTargets();

	if (depthState_[currentDepthStateType_].pDepthStencilState_)
	{
		pContext_->OMSetDepthStencilState(
			depthState_[currentDepthStateType_].pDepthStencilState_.Get(),
			1
		);
	}
	else
	{
		pContext_->OMSetDepthStencilState(nullptr, 1);
	}
}
