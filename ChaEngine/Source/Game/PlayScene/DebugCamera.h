#pragma once
#include <Game/Game.h>

struct DebugCameraData
{
	Vec2 angles{};
	float moveSpeedPerSec{ 0.001f };
	float rotateSpeedPerSec{ 1.0f };
	bool isHideCursor{ false };  // カーソルを隠すか
};

struct DebugCamera : ObjectBase<DebugCameraData>
{
	void Start() override;
	void Update() override;
};
