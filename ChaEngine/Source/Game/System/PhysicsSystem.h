#pragma once
#include <Base/Base.h>
#include "../Component/StaticOBB.h"

/// @brief 物理システムのデータ
struct PhysicsSystemData
{
	std::set<GameObject> checked_{};
	std::vector<GameObject> calculationQueue_{};
};

/// @brief 物理システム
struct PhysicsSystem : SystemObjectBase<PhysicsSystem, PhysicsSystemData>
{
	void Update();

	/// @brief 剛体の更新をする
	void UpdateRigidBody();

private:
	/// @brief OBB同士での当たり判定
	/// @return あたっている true / false
	bool IsHitOBB2OBB(StaticOBB& obb1, StaticOBB& obb2);

	/// @brief 分離軸に投影された軸成分から投影線分長を算出
	/// @param sep 
	/// @param e1 
	/// @param e2 
	/// @param e3 
	/// @return 
	float LenSegOnSeparateAxis(
		const Vec3 sep,
		const Vec3 e1,
		const Vec3 e2,
		const Vec3* e3 = nullptr);

	/// @brief 押し出しを取得する
	/// @param obb1 
	/// @param obb2 
	/// @param outPushVector 
	/// @return 
	bool GetPushOBB2OBB(StaticOBB& obb1, StaticOBB& obb2, Vec3& outPushVector);
};
