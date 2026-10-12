#pragma once
#include <Game/Game.h>

/// @brief レイキャストであたった対象の情報
struct RaycastHitInfo
{
	Transform* pTransform{ nullptr };
	StaticOBB* pOBB{ nullptr };
	IObject* pObject{ nullptr };
	float hitDistance{ FLT_MAX };
	Vec3 hitPosition{};
};
