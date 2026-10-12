#pragma once
#include <Base/Base.h>
#include <Base/Manager/Dx11Manager.h>
#include "Transform.h"

/// @brief その場で計算する動的な座標系コンポーネント
struct DynamicTransform : ComponentBase<DynamicTransform, Transform>
{
	/// @brief Transform のマテリアルを取得
	/// @return マテリアルの参照ポインタ
	inline const Resource<ResourceType::Material>& GetMaterial() override
	{
		using namespace DirectX;

		MaterialResource& transformMaterial = material.Get();
		transformMaterial.slot = 2;
		if (!transformMaterial.pProperty)
		{
			transformMaterial.SetProperty<TransformMaterialProperty>();
		}

		TransformMaterialProperty* pProperty =
			static_cast<TransformMaterialProperty*>(transformMaterial.pProperty.get());

		(*pProperty)->normal = normal = XMMatrixIdentity()
			* XMMatrixRotationQuaternion(rotation);

		(*pProperty)->world = world = XMMatrixIdentity()
			* XMMatrixScalingFromVector(scale)
			* XMMatrixRotationQuaternion(rotation)
			* XMMatrixTranslationFromVector(position);

		return material;
	}
};
