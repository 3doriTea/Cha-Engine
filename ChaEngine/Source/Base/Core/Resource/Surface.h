#pragma once
#include "ResourceRegistryBase.h"

namespace cha
{
	/// @brief サーフェースリソース
	struct SurfaceResource
	{
		ComPtr<ID3D11Buffer> pVertexBuffer{};
		ComPtr<ID3D11Buffer> pIndexBuffer{};
		UINT size{};
		UINT offset{};
		UINT vertexCount{};
		UINT indexCount{};
		D3D11_PRIMITIVE_TOPOLOGY toplogy{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	};

	/// @brief サーフェースリソースのレジストリ
	class SurfaceRegistry : public ResourceRegistryBase<SurfaceResource>
	{
	public:
		SurfaceRegistry() = default;
		~SurfaceRegistry() = default;

		/// @brief 試しにサーフェースをロードする
		/// @param _file サーフェースファイルのパス
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		bool TryLoad(fs::path&& _file, SurfaceResource& _out) override { return false; }
	};

	template<> struct ResourceTraits<ResourceType::Surface>
	{
		using DataT = SurfaceResource;
		using RegistryT = SurfaceRegistry;
		using LoadConfigT = fs::path;
	};
}
