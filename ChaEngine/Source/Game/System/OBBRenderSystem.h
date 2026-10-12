#pragma once
#include <Base/Base.h>

struct OBBRenderSystemData
{
	Resource<ResourceType::Shader> debugLineShader
	{
		"Shader/DebugLine.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_NONE, .fillMode = D3D11_FILL_WIREFRAME }
	};
	Resource<ResourceType::Model> cubeModel{ "Model/UnitCube/UnitCube.fbx" };
	Resource<ResourceType::Material> debugOBBMaterial{};
	Resource<ResourceType::Surface> cubeSuraface{};
};

struct OBBRenderSystem : SystemObjectBase<OBBRenderSystem, OBBRenderSystemData>
{
	/// @brief 初期化処理
	void Start() override;

	/// @brief 描画処理
	void Draw() override;
};
