
#include "LightManager.h"
#include "LightManager/LightMaterial.h"
#include "Dx11Manager.h"

cha::LightManager::LightManager(Dx11Manager& _dx11Manager) :
	dx11Manager_{ _dx11Manager },
	direction_{ Vec3::Down() },
	shadowRange{ 80.0f }
{
	{  // マテリアル作成
		MaterialResource mat{ 10 }; // スロット0
		mat.SetProperty<LightMaterialProperty>(); // 内部で安全に unique_ptr が作られる

		// 初期値 Identity をセット
		LightMaterialProperty* pProp = static_cast<LightMaterialProperty*>(mat.pProperty.get());
		pProp->Get().worldViewProjection = DirectX::XMMatrixIdentity();

		// リソースに格納
		lightMaterial_ = Resource<ResourceType::Material>{ std::move(mat) };
	}
}

bool cha::LightManager::Update()
{
	MaterialResource& matRef = const_cast<MaterialResource&>(lightMaterial_.Get());
	LightMaterialProperty* pProp = static_cast<LightMaterialProperty*>(matRef.pProperty.get());

	// ライトの方向を取得
	Vec3 dir = direction_.Normalize();

	// ライトの座標
	Vec3 target = centerPosition;
	// ライトを十分に離した位置に配置する
	float distance = 100.0f;
	Vec3 pos = target - (dir * distance);

	// ライト用のビュー行列を作成
	Vec3 up = { 0.0f, 0.0f, 1.0f };
	
	// ビュー行列を取得
	Mat4x4 view = DirectX::XMMatrixLookAtLH(pos, target, up);

	// ライト用のプロジェクション行列（正射影）を作成
	// 平行光源のため遠近感のないOrthographicを使う
	float halfRange = shadowRange;
	Mat4x4 proj = DirectX::XMMatrixOrthographicLH(halfRange, halfRange, nearClipDistance, farClipDistance);

	pProp->Get().worldViewProjection = view * proj;

	return true;
}

bool cha::LightManager::Bind()
{
	dx11Manager_.SetMaterial(lightMaterial_);

	return true;
}
