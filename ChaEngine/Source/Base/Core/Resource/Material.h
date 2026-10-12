#pragma once
#include "ResourceRegistryBase.h"
#include "Material/IMaterialProperty.h"
#include "Texture.h"
#include "Shader.h"
#include "../Resource.h"

namespace cha
{
	class IMaterialProperty;

	struct MaterialResource
	{
		// TODO: このUINTをMaterialSlotTypeとかにして強制的にマジックナンバーを消す
		inline MaterialResource(const UINT _slot = 0) :
			slot{ _slot }
		{}

		// コピー禁止
		MaterialResource(const MaterialResource&) = delete;
		MaterialResource& operator=(const MaterialResource& _src) = delete;
		/*MaterialResource(const MaterialResource&) = default;
		inline MaterialResource& operator=(const MaterialResource& _src)
		{
			slot = _src.slot;
			pConstantBuffer = std::move(_src.pConstantBuffer);
			pProperty = std::move(_src.pProperty);
		}*/

		// ムーブ許可
		MaterialResource(MaterialResource&&) noexcept = default;
		MaterialResource& operator=(MaterialResource&&) noexcept = default;

		
		UINT slot{ 0 };
		ComPtr<ID3D11Buffer> pConstantBuffer{};
		Resource<ResourceType::Shader> shader{};
		Resource<ResourceType::Texture> texture{};
		std::unique_ptr<IMaterialProperty> pProperty{};
		// TODO: TextureとShaderとMaterialBaseのポインタを持っておく
		//     : マテリアルプロパティベースのほうがいい

		bool TryLoad(std::unique_ptr<IMaterialProperty>&& _pProperty);

		template<typename PropertyT, typename ...Args>
		inline void SetProperty(Args&& ...args)
		{
			bool succeed = TryLoad(std::move(std::make_unique<PropertyT>(std::forward<Args>(args)...)));
			VCU_ASSERT(succeed && "マテリアルのプロパティ読み込み失敗");
		}

		template<typename PropertyT, typename ...Args>
		inline static MaterialResource InstanceEmpty(Args&& ...args)
		{
			MaterialResource material{};
			material.template SetProperty<PropertyT>(std::forward<Args>(args)...);

			return material;
		}
	};

	/// @brief マテリアルリソースを格納
	class MaterialRegistry : public ResourceRegistryBase<MaterialResource>
	{
	public:
		MaterialRegistry() = default;
		~MaterialRegistry() = default;
	};

	template<> struct ResourceTraits<ResourceType::Material>
	{
		using DataT = MaterialResource;
		using RegistryT = MaterialRegistry;
		using LoadConfigT = fs::path;
	};
}
