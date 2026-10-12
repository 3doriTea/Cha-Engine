
#include "RaycastSystem.h"

bool RaycastSystem::Raycast(const Vec3 _origin, const Vec3 _direction, const float _length, RaycastHitInfo* _pOut)
{
	// 使用例
	auto viewStatic = GetView<Transform, StaticOBB>();

	float minDistance{ FLT_MAX };
	bool isHit{ false };
	viewStatic.ForEach([this, _pOut, &_origin, &_direction, _length, &isHit, &minDistance](IObject& _object, Transform& _transform, StaticOBB& _obb)
	{
		// Transformを渡して、その場でワールド空間のOBBを作る
		DirectX::BoundingOrientedBox worldOBB = ConvertToDirectXOBB(_obb, _transform);

		// Raycast判定
		float dist = 0.0f;
		if (worldOBB.Intersects(_origin, _direction, dist))
		{
			if (dist > _length)
			{
				return;  // 距離が範囲外なら無視
			}

			if (dist < minDistance)
			{
				minDistance = dist;

				isHit = true;
				if (_pOut)
				{
					_pOut->pTransform = &_transform;
					_pOut->pOBB = &_obb;
					_pOut->pObject = &_object;
				}
			}
		}
	});

	if (!isHit)
	{
		return false;  // 当たってない
	}

	if (_pOut)
	{
		// 当たった距離を測定
		_pOut->hitDistance = minDistance;
		// あたった場所も求められる
		_pOut->hitPosition = _origin + _direction * _pOut->hitDistance;
	}
	return true;  // 当たった！
}

DirectX::BoundingOrientedBox RaycastSystem::ConvertToDirectXOBB(const StaticOBB& _obb, const Transform& _transform)
{
	using namespace DirectX;

	BoundingOrientedBox dxObb;

	// 1. 中心位置のワールド変換 (Transformのworld行列にOBBのoffsetを適用)
	XMVECTOR worldPos = XMVector3TransformCoord(_obb.offset, _transform.world);
	XMStoreFloat3(&dxObb.Center, worldPos);

	// 2. 各軸の長さのワールド変換 (Transformのスケール × OBBの個別スケール)
	// ※現在のPhysicsSystem::Updateの計算（各軸の掛け算）をそのまま再現しています
	dxObb.Extents.x = _transform.scale.v[0] * _obb.scale.v[0];
	dxObb.Extents.y = _transform.scale.v[1] * _obb.scale.v[1];
	dxObb.Extents.z = _transform.scale.v[2] * _obb.scale.v[2];

	// 3. 方向ベクトルのワールド変換とクォータニオン化
	// Transformからワールド空間の右・上・前方向ベクトルを取得して正規化
	XMVECTOR right = XMVector3Normalize(_transform.Right());
	XMVECTOR up = XMVector3Normalize(_transform.Up());
	XMVECTOR forward = XMVector3Normalize(_transform.Forward());

	// 3本のベクトルから回転行列を作成
	XMMATRIX rotationMatrix = XMMatrixSet(
		XMVectorGetX(right), XMVectorGetY(right), XMVectorGetZ(right), 0.0f,
		XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up), 0.0f,
		XMVectorGetX(forward), XMVectorGetY(forward), XMVectorGetZ(forward), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// 行列からクォータニオンを抽出して設定
	XMVECTOR quaternion = XMQuaternionRotationMatrix(rotationMatrix);
	XMStoreFloat4(&dxObb.Orientation, quaternion);

	return dxObb;
}
