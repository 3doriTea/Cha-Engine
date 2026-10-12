#pragma once
#include <Game/Game.h>

struct EntityData
{
	Component<Transform> transform{};

};

struct Entity : ObjectBase<EntityData>
{
};
