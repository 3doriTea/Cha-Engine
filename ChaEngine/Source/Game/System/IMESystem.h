#pragma once
#include <Game/Game.h>

struct IMESystemData
{

};

struct IMESystem : SystemObjectBase<IMESystem, IMESystemData>
{
	void Start() override;
	void Update() override;
};
