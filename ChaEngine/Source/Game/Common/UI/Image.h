#pragma once
#include <Game/Game.h>

namespace UI
{
	struct ImageData
	{
		ImageData(const std::string_view _imageFile) :
			texture{ _imageFile }
		{
		}

		Resource<ResourceType::Shader> simpleShader
		{
			"Shader/SimpleSqure.hlsl",
			RasterizerConfig{ .cullMode = D3D11_CULL_BACK, .frontCounterClockwise = false }
		};
		Resource<ResourceType::Surface> triangleSuraface{};
		Resource<ResourceType::Texture> texture;
	};

	struct Image : ObjectBase<ImageData>
	{
		using ObjectBase<ImageData>::ObjectBase;

		void Start() override;
		void Draw() override;
	};
}
