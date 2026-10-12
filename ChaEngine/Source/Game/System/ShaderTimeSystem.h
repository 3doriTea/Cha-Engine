#pragma once
#include <Game/Game.h>

/// @brief シェーダへ時間を送るシステムのデータ
struct ShaderTimeSystemData
{
	Resource<ResourceType::Material> timeMaterial{};

	float time{};
};

/// @brief シェーダへ時間を送るシステム
struct ShaderTimeSystem : SystemObjectBase<ShaderTimeSystem, ShaderTimeSystemData>
{
	void Start() override;
	void Update() override;
};
