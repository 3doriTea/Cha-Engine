
#include "Transform.h"

void Transform::UpdateFamily()
{
	using namespace DirectX;

	SceneManager& sceneManager = ManagerRegistry::Ref<SceneManager>();

	Transform* pParentTransform{ nullptr };
	if (!parent.IsEmpty())  // 軽量な有無チェック
	{
		pParentTransform = sceneManager.FindComponent<Transform>(parent);
		if (pParentTransform)
		{
			// 親がTransformコンポーネントを持っているなら親を先に更新
			pParentTransform->UpdateFamily();
		}
	}

	UpdateSelf();

	if (pParentTransform)
	{
		// 親がいるなら親の行列を掛ける
		normal = localNormal * pParentTransform->normal;
		world = local * pParentTransform->world;
	}
}

void Transform::UpdateSelf()
{
	using namespace DirectX;

	// 法線方向の行列を計算する
	localNormal = XMMatrixIdentity()
		* XMMatrixRotationQuaternion(rotation);

	// 座標変換行列を計算する
	local = XMMatrixIdentity()
		* XMMatrixScalingFromVector(scale)
		* XMMatrixRotationQuaternion(rotation)
		* XMMatrixTranslationFromVector(position);
}
