#pragma once
#include "ResourceRegistryBase.h"

namespace cha
{
	/// @brief デフォルトのサンプラー設定
	constexpr D3D11_SAMPLER_DESC DEFAULT_SAMPER_DESC
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MipLODBias = {},
		.MaxAnisotropy = {},
		.ComparisonFunc = D3D11_COMPARISON_NEVER,
		.BorderColor = {},
		.MinLOD = 0,
		.MaxLOD = 0,
	};

	/// @brief テクスチャリソース
	struct TextureResource
	{
		ComPtr<ID3D11SamplerState> pSamplerState{};  ///< サンプラーステート
		ComPtr<ID3D11ShaderResourceView> pSRV{};     ///< シェーダリソースビュー
		Vec2 imageSize{};
		std::vector<fs::path> files{};
		D3D11_SAMPLER_DESC samplerDesc{};
		inline HashValue GetHash() const { return 0; }
	};

	/// @brief テクスチャの種類
	enum TextureType
	{
		TextureType_Image,           ///< ただの画像
		TextureType_Image_InMemory,  ///< ただの画像(メモリ上)
		TextureType_SkyBox,     ///< スカイボックス用6面
		TextureType_ShadowMap,  ///< シャドウマップ
	};

	/// @brief テクスチャの読み込み設定
	struct TextureLoadConfig
	{
		/// @brief ただの画像
		/// @param _file ただの画像1枚
		inline TextureLoadConfig(
			const fs::path& _file,
			const bool _generateMipMap = false,
			const D3D11_SAMPLER_DESC _samplerDesc = DEFAULT_SAMPER_DESC)
		{
			generateMipMap = _generateMipMap;
			samplerDesc = _samplerDesc;
			textureType = TextureType_Image;
			files.push_back(_file);
		}

		/// @brief スカイスフィア用6面
		/// @param _type テクスチャの種類
		/// @param _files6 6枚分の画像
		/// @param _samplerDesc サンプラーの設定
		inline TextureLoadConfig(
			const TextureType _type,
			const std::array<const fs::path, 6>& _files6,
			const D3D11_SAMPLER_DESC _samplerDesc = DEFAULT_SAMPER_DESC)
		{
			samplerDesc = _samplerDesc;

			VCU_ASSERT((_type == TextureType_SkyBox)
				&& "テクスチャタイプだけの設定はシャドウマップのみ指定できる");
			
			textureType = TextureType_SkyBox;
			for (auto& file : _files6)
			{
				files.push_back(file);
			}
		}

		/// @brief スカイスフィア用6面
		/// @param _files6 6枚の画像
		inline TextureLoadConfig(const TextureType _type, ComPtr<ID3D11Texture2D>& _pTexture2D)
		{
			pTexture2D = _pTexture2D;
			VCU_ASSERT((_type == TextureType_ShadowMap)
				&& "テクスチャタイプだけの設定はシャドウマップのみ指定できる");

			textureType = TextureType_ShadowMap;
		}

		/// @brief メモリ上から画像を読み込む
		/// @param _pMemoryData 
		/// @param _memoryDataSize 
		/// @param _file 
		inline TextureLoadConfig(
			const void* _pMemoryData,
			const size_t _memoryDataSize,
			const fs::path& _file) :
			pMemoryData{ _pMemoryData }
		{
			memoryDataSize = _memoryDataSize;
			files.push_back(_file);

			textureType = TextureType_Image_InMemory;
		}

		TextureType textureType{ TextureType_Image };
		std::vector<fs::path> files{};
		D3D11_SAMPLER_DESC samplerDesc{ DEFAULT_SAMPER_DESC };
		bool generateMipMap{};
		Vec2 textureSize{};  ///< テクスチャのサイズ
		ComPtr<ID3D11Texture2D> pTexture2D{};  ///< 2Dテクスチャー
		const void* pMemoryData{ nullptr };  ///< メモリ上から読み込むときのポインタ
		size_t memoryDataSize{ 0 };    ///< メモリ上から読み込むときのサイズ
	};

	class TextureRegistry : public ResourceRegistryBase<TextureResource, TextureLoadConfig>
	{
	public:
		TextureRegistry();
		~TextureRegistry() = default;

		bool TryLoad(TextureLoadConfig&& _config, TextureResource& _out) override;
	};

	template<> struct ResourceTraits<ResourceType::Texture>
	{
		using DataT = TextureResource;
		using RegistryT = TextureRegistry;
		using LoadConfigT = TextureLoadConfig;
	};
}
