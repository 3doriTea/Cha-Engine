#include "PhysicsSystem.h"
#include "../Component/StaticOBB.h"
#include "../Component/DynamicOBB.h"
#include "../Component/Transform.h"
#include "../Component/RigidBody.h"
#include <Base/Manager/DeltaTime.h>

void PhysicsSystem::Update()
{
	using namespace DirectX;

	auto viewStatic = GetView<Transform, StaticOBB>();
	auto viewDynamic = GetView<Transform, DynamicOBB>();

#pragma region コンポーネントの値更新
	viewStatic.ForEach([](IObject& _object, Transform& _transform, StaticOBB& _obb)
	{
		// 1-1. 中心位置
		_obb.position = XMVector3TransformCoord(_obb.offset, _transform.world);

		// ワールド行列から各軸のベクトルを取得
		Vec3 worldRight = _transform.world.r[0];
		Vec3 worldUp = _transform.world.r[1];
		Vec3 worldForward = _transform.world.r[2];

		Vec3 obbScale = _obb.scale;
		worldRight = XMVectorScale(worldRight, XMVectorGetX(obbScale));
		worldUp = XMVectorScale(worldUp, XMVectorGetY(obbScale));
		worldForward = XMVectorScale(worldForward, XMVectorGetZ(obbScale));

		// 1-3. 各軸の長さ
		_obb.axisLength.v[0] = XMVector3Length(worldRight).m128_f32[0];
		_obb.axisLength.v[1] = XMVector3Length(worldUp).m128_f32[0];
		_obb.axisLength.v[2] = XMVector3Length(worldForward).m128_f32[0];

		// 1-2. 各方向ベクトル
		_obb.normalDirect[0] = XMVector3Normalize(worldRight);
		_obb.normalDirect[1] = XMVector3Normalize(worldUp);
		_obb.normalDirect[2] = XMVector3Normalize(worldForward);
	});

	viewDynamic.ForEach([](IObject& _object, Transform& _transform, DynamicOBB& _obb)
	{
		_obb.onGrounded = false;

		// 2-1. 中心位置
		_obb.position = XMVector3TransformCoord(_obb.offset, _transform.world);

		// 2-2. 各方向ベクトル
		_obb.normalDirect[0] = XMVector3Normalize(_transform.Right());
		_obb.normalDirect[1] = XMVector3Normalize(_transform.Up());
		_obb.normalDirect[2] = XMVector3Normalize(_transform.Forward());

		// 2-3. 各軸の長さ
		_obb.axisLength.v[0] = _transform.scale.v[0] * _obb.scale.v[0];
		_obb.axisLength.v[1] = _transform.scale.v[1] * _obb.scale.v[1];
		_obb.axisLength.v[2] = _transform.scale.v[2] * _obb.scale.v[2];
	});
#pragma endregion

	std::map<Transform*, Vec3> refrections{};  // 押し出す力
	viewDynamic.ForEach([&viewStatic, this, &refrections](IObject& _objectSelf, Transform& _transformSelf, DynamicOBB& _dynamicOBBSelf)
	{
		_dynamicOBBSelf.hitGameObjects.clear();

		viewStatic.ForEach([&viewStatic, this, &refrections, &_objectSelf, &_transformSelf, &_dynamicOBBSelf]
		(IObject& _objectOther, Transform& _transformOther, StaticOBB& _staticOBBOther)
		{
			if (&_objectSelf == &_objectOther)
			{
				return;  // 同じオブジェクトは無視
			}

			Vec3 pushVector{};
			if (GetPushOBB2OBB(_dynamicOBBSelf, _staticOBBOther, pushVector))
			{
				// 単純加算（+=）すると複数ブロック接触時に押し出し倍増して宙に浮くため、
				// 各軸ごとに最も絶対値が大きい補正量のみを採用する
				Vec3& currentPush = refrections[&_transformSelf];

				if (std::abs(pushVector.x) > std::abs(currentPush.x)) currentPush.x = pushVector.x;
				if (std::abs(pushVector.y) > std::abs(currentPush.y)) currentPush.y = pushVector.y;
				if (std::abs(pushVector.z) > std::abs(currentPush.z)) currentPush.z = pushVector.z;

				_dynamicOBBSelf.hitGameObjects.emplace_back(_objectOther.ToGameObject(), pushVector);
			}
		});
	});

	// 実際に計算された押し出し力をTransformに適用してめり込みを解決する
	for (auto& [pTransform, pushForce] : refrections)
	{
		// 座標をめり込んでいる分だけ押し戻す
		pTransform->position += pushForce;
	}

	// 着地判定の更新
	viewDynamic.ForEach([&viewStatic, this, &refrections]
	(IObject& _objectSelf, Transform& _transformSelf, DynamicOBB& _dynamicOBBSelf)
	{
		// 上方向への押し出しが発生していれば接地状態とみなす
		if (refrections[&_transformSelf].y > 0.0001f)
		{
			_dynamicOBBSelf.onGrounded = true;
		}
	});

	UpdateRigidBody();
}

void PhysicsSystem::UpdateRigidBody()
{
	const float DELTA_TIME = Get<DeltaTime>();

	// TODO: 簡易的すぎる速度の求め方
	// TODO: 将来的には回転も考慮し、台車が傾いた方向への力も台車根本基準で求めたりする
	auto viewRigidBody = GetView<Transform, RigidBody>();
	viewRigidBody.ForEach([DELTA_TIME](IObject& _object, Transform& _transform, RigidBody& _rigidBody)
	{
		Vec3 currentWorldPosition = DirectX::XMVector3TransformCoord(
			Vec3::Zero(),
			_transform.world);

		// 単純に前回のフレームとの差分から速度を求める
		_rigidBody.velocity = (currentWorldPosition - _rigidBody.previousWorldPosition) / DELTA_TIME;
		_rigidBody.previousWorldPosition = currentWorldPosition;
	});
}

bool PhysicsSystem::IsHitOBB2OBB(StaticOBB& obb1, StaticOBB& obb2)
{
	// REF: http://marupeke296.com/COL_3D_No13_OBBvsOBB.html

	using DirectX::XMVectorGetX;
	using DirectX::XMVector3Dot;
	using DirectX::XMVector3Cross;

	// 各方向ベクトルの確保
	// （N***:標準化方向ベクトル）
	Vec3 NAe1 = obb1.normalDirect[0], Ae1 = NAe1 * obb1.axisLength.v[0];
	Vec3 NAe2 = obb1.normalDirect[1], Ae2 = NAe2 * obb1.axisLength.v[1];
	Vec3 NAe3 = obb1.normalDirect[2], Ae3 = NAe3 * obb1.axisLength.v[2];
	Vec3 NBe1 = obb2.normalDirect[0], Be1 = NBe1 * obb2.axisLength.v[0];
	Vec3 NBe2 = obb2.normalDirect[1], Be2 = NBe2 * obb2.axisLength.v[1];
	Vec3 NBe3 = obb2.normalDirect[2], Be3 = NBe3 * obb2.axisLength.v[2];
	Vec3 Interval = obb1.position - obb2.position;  // 座標の差分

	// 分離軸 : Ae1
	FLOAT rA = Ae1.Length();
	FLOAT rB = LenSegOnSeparateAxis(NAe1, Be1, Be2, &Be3);
	FLOAT L = fabs(XMVectorGetX(XMVector3Dot(Interval, NAe1)));
	if (L > rA + rB)
		return false; // 衝突していない

	// 分離軸 : Ae2
	rA = Ae2.Length();
	rB = LenSegOnSeparateAxis(NAe2, Be1, Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, NAe2)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Ae3
	rA = Ae3.Length();
	rB = LenSegOnSeparateAxis(NAe3, Be1, Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, NAe3)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(NBe1, Ae1, Ae2, &Ae3);
	rB = Be1.Length();
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, NBe1)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(NBe2, Ae1, Ae2, &Ae3);
	rB = Be2.Length();
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, NBe2)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(NBe3, Ae1, Ae2, &Ae3);
	rB = Be3.Length();
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, NBe3)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C11
	Vec3 Cross = XMVector3Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C12
	Cross = XMVector3Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C13
	Cross = XMVector3Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C21
	Cross = XMVector3Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C22
	Cross = XMVector3Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C23
	Cross = XMVector3Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C31
	Cross = XMVector3Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C32
	Cross = XMVector3Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C33
	Cross = XMVector3Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LenSegOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(Interval, Cross)));
	if (L > rA + rB)
		return false;

	// 分離平面が存在しないので「衝突している」
	return true;
}

float PhysicsSystem::LenSegOnSeparateAxis(const Vec3 sep, const Vec3 e1, const Vec3 e2, const Vec3* e3)
{
	using DirectX::XMVectorGetX;
	using DirectX::XMVector3Dot;

	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸sepは標準化されていること
	float r1 = fabs(XMVectorGetX(XMVector3Dot(sep, e1)));
	float r2 = fabs(XMVectorGetX(XMVector3Dot(sep, e2)));
	float r3 = e3 ? (fabs(XMVectorGetX(XMVector3Dot(sep, *e3)))) : 0.0f;
	return r1 + r2 + r3;
}

bool PhysicsSystem::GetPushOBB2OBB(StaticOBB& obb1, StaticOBB& obb2, Vec3& outPushVector)
{
	using DirectX::XMVectorGetX;
	using DirectX::XMVector3Dot;
	using DirectX::XMVector3Cross;
	using DirectX::XMVector3Normalize;

	// 各方向ベクトルの確保
	Vec3 NAe1 = obb1.normalDirect[0], Ae1 = NAe1 * obb1.axisLength.v[0];
	Vec3 NAe2 = obb1.normalDirect[1], Ae2 = NAe2 * obb1.axisLength.v[1];
	Vec3 NAe3 = obb1.normalDirect[2], Ae3 = NAe3 * obb1.axisLength.v[2];
	Vec3 NBe1 = obb2.normalDirect[0], Be1 = NBe1 * obb2.axisLength.v[0];
	Vec3 NBe2 = obb2.normalDirect[1], Be2 = NBe2 * obb2.axisLength.v[1];
	Vec3 NBe3 = obb2.normalDirect[2], Be3 = NBe3 * obb2.axisLength.v[2];
	Vec3 Interval = obb1.position - obb2.position;

	// 最短押し出し情報を記録する変数
	float minOverlap = FLT_MAX;
	Vec3 pushAxis = Vec3(0, 0, 0);

	// 各軸の判定用ラムダ式（コードの重複を減らし、正規化とめり込み計算を共通化）
	auto TestAxis = [&](Vec3 axis) -> bool
	{
		// 軸の長さが0に近い（外積で平行になった場合など）なら無視
		float axisLen = axis.Length();
		if (axisLen < 0.0001f) return true;

		// 💡 影の長さを正しく測るために軸を正規化する
		axis = XMVector3Normalize(axis);

		float rA = LenSegOnSeparateAxis(axis, Ae1, Ae2, &Ae3);
		float rB = LenSegOnSeparateAxis(axis, Be1, Be2, &Be3);
		float L = fabs(XMVectorGetX(XMVector3Dot(Interval, axis)));

		if (L > rA + rB)
		{
			return false; // 分離平面を発見（衝突していない）
		}

		// めり込み深さを計算
		float overlap = (rA + rB) - L;
		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			pushAxis = axis;
		}
		return true;
	};

	// 15本の軸を順番にテスト
	if (!TestAxis(NAe1)) return false;
	if (!TestAxis(NAe2)) return false;
	if (!TestAxis(NAe3)) return false;

	if (!TestAxis(NBe1)) return false;
	if (!TestAxis(NBe2)) return false;
	if (!TestAxis(NBe3)) return false;

	if (!TestAxis(XMVector3Cross(NAe1, NBe1))) return false;
	if (!TestAxis(XMVector3Cross(NAe1, NBe2))) return false;
	if (!TestAxis(XMVector3Cross(NAe1, NBe3))) return false;

	if (!TestAxis(XMVector3Cross(NAe2, NBe1))) return false;
	if (!TestAxis(XMVector3Cross(NAe2, NBe2))) return false;
	if (!TestAxis(XMVector3Cross(NAe2, NBe3))) return false;

	if (!TestAxis(XMVector3Cross(NAe3, NBe1))) return false;
	if (!TestAxis(XMVector3Cross(NAe3, NBe2))) return false;
	if (!TestAxis(XMVector3Cross(NAe3, NBe3))) return false;

	if (XMVectorGetX(XMVector3Dot(Interval, pushAxis)) < 0.0f)
	{
		pushAxis = -pushAxis;
	}

	// 押し出すベクトル（方向 × 深さ）を確定させる
	outPushVector = pushAxis * minOverlap;

	return true;
}
