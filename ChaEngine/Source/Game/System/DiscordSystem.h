#pragma once
#include <Base/Base.h>

struct DiscordSystemData
{
};

struct DiscordSystem : SystemObjectBase<DiscordSystem, DiscordSystemData>
{
	void Start() override;
	void Update() override;
};
