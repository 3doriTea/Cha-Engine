#pragma once
#include "../Game.h"

struct SkyData
{
	Resource<ResourceType::Texture> skyboxTextureSpace
	{
		TextureLoadConfig
		{
			TextureType_SkyBox,
			std::array<const fs::path, 6>
			{
				"Texture/SkyBox/Standard-Cube-Map (1)/px.png",  // +X
				"Texture/SkyBox/Standard-Cube-Map (1)/nx.png",  // -X
				"Texture/SkyBox/Standard-Cube-Map (1)/py.png",  // +Y
				"Texture/SkyBox/Standard-Cube-Map (1)/ny.png",  // -Y
				"Texture/SkyBox/Standard-Cube-Map (1)/pz.png",  // +Z
				"Texture/SkyBox/Standard-Cube-Map (1)/nz.png",  // -Z
			}
		}
	};

	Resource<ResourceType::Texture> skyboxTextureDay
	{
		TextureLoadConfig
		{
			TextureType_SkyBox,
			std::array<const fs::path, 6>
			{
				"Texture/SkyBox/Daylight Box_Pieces/Daylight Box_Right.bmp",  // +X
				"Texture/SkyBox/Daylight Box_Pieces/Daylight Box_Left.bmp",  // -X
				"Texture/SkyBox/Daylight Box_Pieces/Daylight Box_Top.bmp",  // +Y
				"Texture/SkyBox/Daylight Box_Pieces/Daylight Box_Bottom.bmp",  // -Y
				"Texture/SkyBox/Daylight Box_Pieces/Daylight Box_Front.bmp",  // +Z
				"Texture/SkyBox/Daylight Box_Pieces/Daylight Box_Back.bmp",  // -Z
			}
	} };

	Resource<ResourceType::Shader> skyBoxShader
	{
		"Shader/SkyBox.hlsl",
		RasterizerConfig
		{
			.cullMode = D3D11_CULL_BACK,    // 両面描画
			.fillMode = D3D11_FILL_SOLID,
			.frontCounterClockwise = false,
		}
	};
	Resource<ResourceType::Material> material{};
	Resource<ResourceType::Surface> skyBoxSurface{};  // スカイボックスの面

	float spaceAngleX;  // 宇宙の回転x軸
	float spaceAngleY;  // 宇宙の回転y軸
	float spaceAngleZ;  // 宇宙の回転z軸

	Float4 skyTime{ 0.0f, 0.0f, 0.0f, 0.0f };  // (x時間 y時間 z時間 w時間)

	static constexpr size_t VERTEX_COUNT{ 8 };
	static constexpr size_t INDEX_COUNT{ 36 };
};

struct Sky : ObjectBase<SkyData>
{
	void Start() override;
	void Update() override;
	void Draw() override;
};
