#pragma once
#include <Base/Base.h>
#include <Base/Manager/ScreenSize.h>
#include <Base/Manager/UIManager/UILayerOrder.h>
#include "UITransform/UITransformMaterial.h"

/// @brief UI座標系コンポーネント
struct UITransform : ComponentBase<UITransform>
{
	inline UITransform() :
		UITransform{ Vec2::Zero(), Vec2::Zero() }
	{}

	inline UITransform(const Vec2 _begin, const Vec2 _scale, UILayerOrder _layerOrder = 0) :
		position{ _begin },
		scale{ _scale },
		layerOrder{ _layerOrder }
	{}

	/// @brief Transform のマテリアルを取得
	/// @return マテリアルの参照ポインタ
	inline const Resource<ResourceType::Material>& GetMaterial()
	{
		MaterialResource& transformMaterial = material.Get();
		transformMaterial.slot = 2;
		if (!transformMaterial.pProperty)
		{
			transformMaterial.SetProperty<UITransformMaterialProperty>();
		}

		UITransformMaterialProperty* pProperty =
			static_cast<UITransformMaterialProperty*>(transformMaterial.pProperty.get());

		// スクリーンのサイズ
		const Vec2 SCREEN_SIZE = ManagerRegistry::Ref<ScreenSize>();

		// NCD座標に変換する行列を作る
		(*pProperty)->ncd =
			uiWorld
			* DirectX::XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_SIZE.x, SCREEN_SIZE.y, 0.0f, 0.0f, 1.0f);
		return material;
	}

	Vec2 position{};          // 基準点の座標
	Vec2 scale{};                  // 基準点からの大きさ
	UILayerOrder layerOrder{ 0 };  // レイヤーオーダー

	Mat4x4 uiLocal{};  // UIローカル行列
	Mat4x4 uiWorld{};  // UIワールド行列

	GameObject parent{};  // 親ゲームオブジェクト

private:
	Resource<ResourceType::Material> material{ MaterialResource::InstanceEmpty<UITransformMaterialProperty>() };
};
