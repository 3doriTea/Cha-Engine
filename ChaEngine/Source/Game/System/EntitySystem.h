#pragma once
#include <Game/Game.h>

/// @brief エンティティ統括システムのデータ
struct EntitySystemData
{

};

/// @brief エンティティ統括システム
struct EntitySystem : SystemObjectBase<EntitySystemData>
{
	void Start() override;
	void Update() override;
};
