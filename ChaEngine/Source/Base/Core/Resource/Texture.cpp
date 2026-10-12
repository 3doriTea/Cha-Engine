#include "Texture.h"
#include "../../ManagerRegistry.h"
#include "../../Manager/Dx11Manager.h"


cha::TextureRegistry::TextureRegistry()
{
}

bool cha::TextureRegistry::TryLoad(TextureLoadConfig&& _config, TextureResource& _out)
{
	using namespace DirectX;

	HRESULT hResult{};

	ScratchImage image{};
	TexMetadata metadata{};

	switch (_config.textureType)
	{
	case TextureType_Image_InMemory:
	case TextureType_Image:
	{  // 普通の画像
		// 画像がメモリ上にあるなら
		if (_config.textureType == TextureType_Image_InMemory)
		{
			// メモリ上から読み込む
			hResult = LoadFromWICMemory(
				reinterpret_cast<const uint8_t*>(_config.pMemoryData),  // 生バイナリの先頭ポインタ
				_config.memoryDataSize,                                 // バイトサイズ
				WIC_FLAGS_NONE,
				&metadata,
				image);

			VCU_ASSERT(SUCCEEDED(hResult) && "メモリからの画像読み込みに失敗");
			if (FAILED(hResult))
			{
				return false;
			}
		}
		else  // 画像がファイルとしてあるなら
		{
			// ファイルパスからロード
			hResult = LoadFromWICFile(
				_config.files.front().wstring().c_str(),
				WIC_FLAGS_NONE,
				&metadata,
				image);
			VCU_ASSERT(SUCCEEDED(hResult) && "ファイルからの画像読み込みに失敗");
			if (FAILED(hResult))
			{
				return false;
			}
		}

		// もしフォーマットがアルファ無視ならアルファ付きに変換する
		if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM && false)
		{
			ScratchImage converted{};
			hResult = Convert(image.GetImages(), image.GetImageCount(), metadata,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				TEX_FILTER_DEFAULT,
				TEX_THRESHOLD_DEFAULT,
				converted);

			metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM;

			VCU_ASSERT(SUCCEEDED(hResult) && "画像の透過png変換に失敗");

			image = std::move(converted);
		}

		// ミップマップ生成希望なら
		if (_config.generateMipMap)
		{
			ScratchImage mipChain{};
			if (metadata.mipLevels == 1)
			{
				// TEX_FILTER_BOXやTEX_FILTER_LINEARなど、縮小時のフィルタを選択可能
				hResult = GenerateMipMaps(
					image.GetImages(),
					image.GetImageCount(),
					metadata,
					TEX_FILTER_DEFAULT,
					0, // 0を指定すると、1x1ピクセルになるまでの全階層を自動生成
					mipChain);

				if (SUCCEEDED(hResult))
				{
					// 生成に成功したら、image と metadata をミップマップ付きのものに差し替える
					image = std::move(mipChain);
					metadata = image.GetMetadata();
				}
				else
				{
					VCU_ASSERT(false && "ミップマップの生成に失敗（処理は続行します）");
				}
			}
		}

		ManagerRegistry::Ref<Dx11Manager>().Ref([&hResult, &image, &metadata, &_out](const ComPtr<ID3D11Device>& _pDevice)
		{
			// TODO: 画像フォーマットは今画像データに合わせている
			hResult = CreateShaderResourceView(
				_pDevice.Get(),
				image.GetImages(),
				image.GetImageCount(),
				metadata,
				_out.pSRV.GetAddressOf());
		});

		VCU_ASSERT(SUCCEEDED(hResult) && "テクスチャのSRV作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}

		// サンプラーを作る
		ManagerRegistry::Ref<Dx11Manager>().Ref([this, &hResult, &_out, &_config](const ComPtr<ID3D11Device>& _pDevice)
		{
			_out.samplerDesc = _config.samplerDesc;
			hResult = _pDevice->CreateSamplerState(
				&_out.samplerDesc,
				_out.pSamplerState.GetAddressOf());
		});
		VCU_ASSERT(SUCCEEDED(hResult) && "サンプラーステートの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}

		return true;
	}
	case TextureType_SkyBox:
	{
		// 6枚の画像をメモリに一時読み込み（解像度取得用も兼ねる）
		std::array<ScratchImage, 6> images{};
		std::array<D3D11_SUBRESOURCE_DATA, 6> subResourceData{};

		for (int i = 0; i < 6; ++i)
		{
			// WIC（PNG/JPG等）から読み込み
			hResult = DirectX::LoadFromWICFile(_config.files[i].wstring().c_str(), DirectX::WIC_FLAGS_NONE, nullptr, images[i]);
			if (FAILED(hResult))
			{
				VCU_ASSERT("6面画像のうち1つ失敗");
				return false;
			}

			const DirectX::Image* img = images[i].GetImage(0, 0, 0);

			// サブリソース構造体に、各面の生ピクセル情報を紐付ける
			subResourceData[i].pSysMem = img->pixels;     // ピクセルデータの先頭ポインタ
			subResourceData[i].SysMemPitch = static_cast<UINT>(img->rowPitch); // 1行のバイト数
			subResourceData[i].SysMemSlicePitch = static_cast<UINT>(img->slicePitch); // 1枚の総バイト数
		}

		// 最初の画像から解像度とフォーマットを取得（※6枚すべて同じサイズ・フォーマットである前提）
		const DirectX::Image* baseImg = images[0].GetImage(0, 0, 0);

		// --- 1. キューブマップ用 Texture2D の設定 ---
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = static_cast<UINT>(baseImg->width);
		texDesc.Height = static_cast<UINT>(baseImg->height);
		texDesc.MipLevels = 1;  // ミップマップなし
		texDesc.ArraySize = 6;  // 6面構成
		texDesc.Format = baseImg->format;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ComPtr<ID3D11Texture2D> pCubeTexture{};
		ManagerRegistry::Ref<Dx11Manager>().Ref([this, &hResult, &texDesc, &subResourceData, &pCubeTexture, &_out]
		(const ComPtr<ID3D11Device>& _pDevice)
		{
			hResult = _pDevice->CreateTexture2D(&texDesc, subResourceData.data(), pCubeTexture.GetAddressOf());
		});
		if (FAILED(hResult))
		{
			VCU_ASSERT("6面画像のCreateTexture2Dに失敗");
			return false;
		}

		// --- 2. シェーダーリソースビュー（SRV）の作成 ---
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
		srvDesc.TextureCube.MostDetailedMip = 0;

		ManagerRegistry::Ref<Dx11Manager>().Ref([this, &hResult, &srvDesc, &subResourceData, &pCubeTexture, &_out]
		(const ComPtr<ID3D11Device>& _pDevice)
		{
			hResult = _pDevice->CreateShaderResourceView(pCubeTexture.Get(), &srvDesc, _out.pSRV.GetAddressOf());
		});

		// 使い終わったテクスチャ本体のローカル参照は解放してOK（SRV経由でパイプラインに流すため）
		pCubeTexture.Reset();

		// SkyBox用にサンプラーを作る
		ManagerRegistry::Ref<Dx11Manager>().Ref([this, &hResult, &_out](const ComPtr<ID3D11Device>& _pDevice)
		{
			_out.samplerDesc =
			{
				.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
				.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
				.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
				.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
				.MipLODBias = 0.0f,
				.MaxAnisotropy = 1,
				.ComparisonFunc = D3D11_COMPARISON_ALWAYS,
				.BorderColor = {},
				.MinLOD = 0,
				.MaxLOD = D3D11_FLOAT32_MAX,
			};
			hResult = _pDevice->CreateSamplerState(
				&_out.samplerDesc,
				_out.pSamplerState.GetAddressOf());
		});
		VCU_ASSERT(SUCCEEDED(hResult) && "サンプラーステートの作成に失敗");
		if (FAILED(hResult))
		{
			return false;
		}

		return true;
	}
	case TextureType_ShadowMap:
		{  // シェーダリソースビューも作る
			const D3D11_SHADER_RESOURCE_VIEW_DESC DESC
			{
				.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
				.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
				.Texture2D
				{
					.MostDetailedMip = 0,
					.MipLevels = 1,
			}
			};

			ManagerRegistry::Ref<Dx11Manager>().Ref([this, &hResult, &DESC, &_config, &_out]
			(const ComPtr<ID3D11Device>& _pDevice)
			{
				hResult = _pDevice->CreateShaderResourceView(_config.pTexture2D.Get(), &DESC, _out.pSRV.GetAddressOf());
			});
			if (FAILED(hResult))
			{
				VCU_ASSERT(SUCCEEDED(hResult) && "シャドウマップ深度バッファビューの作成に失敗");
				return false;
			}
		}

		{  // サンプラー
			ManagerRegistry::Ref<Dx11Manager>().Ref([this, &hResult, &_out](const ComPtr<ID3D11Device>& _pDevice)
			{
				_out.samplerDesc =
				{
					.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
					.AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
					.AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
					.AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
					.MipLODBias = 0.0f,
					.MaxAnisotropy = 1,
					.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL,
					.BorderColor = { 1.0f, 1.0f, 1.0f, 1.0f },
					.MinLOD = 0,
					.MaxLOD = D3D11_FLOAT32_MAX,
				};
				hResult = _pDevice->CreateSamplerState(
					&_out.samplerDesc,
					_out.pSamplerState.GetAddressOf());
			});
			VCU_ASSERT(SUCCEEDED(hResult) && "サンプラーステートの作成に失敗");
			if (FAILED(hResult))
			{
				return false;
			}
		}

		//void* ptr = this;

		return true;  // 成功
	default:
		VCU_ASSERT("知らないテクスチャタイプ");
		break;
	}

	return false;
}
