#pragma once
#include <Game/Game.h>

struct TerrainCommonResource
{
	Resource<Shader> shader{ "Shader/Terrain.hlsl" };  ///< 地形描画のシェーダ
	Resource<Texture> texture
	{
		"Texture/Terrain/grass_01.png",
		true,
		D3D11_SAMPLER_DESC
		{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
			.MipLODBias = {},
			.MaxAnisotropy = 1,
			.ComparisonFunc = D3D11_COMPARISON_NEVER,
			.BorderColor = {},
			.MinLOD = 0.0f,
			.MaxLOD = D3D11_FLOAT32_MAX,
	}
	};  ///< 地形のテクスチャ
	Resource<Material> material{};  ///< 地形のマテリアル
};
