#pragma once
#include "../../../BasePch.h"
#include "../../Resource.h"
#include "../Surface.h"
#include "../Material.h"
#include "ModelMaterial.h"
#include "ModelTransformMaterialProperty.h"

namespace cha
{
	template<ResourceType ResourceTypeV> class Resource;

	struct ModelParts
	{
		Resource<ResourceType::Surface> mesh{};
		Resource<ResourceType::Material> material{};
		Resource<ResourceType::Material> transformMaterial
		{
			MaterialResource::InstanceEmpty<ModelTransformMaterialProperty>()
		};
		std::string_view name{};
	};
}
