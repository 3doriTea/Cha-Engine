#pragma once
#include <Game/Game.h>

struct TestAlphaCubeData
{
	Resource<Shader> shaderFront
	{
		"Shader/Unlit.hlsl",
		RasterizerConfig
		{
			.cullMode = D3D11_CULL_FRONT,
			.fillMode = D3D11_FILL_SOLID,
			.frontCounterClockwise = false,
		}
	};  ///< 表シェーダ

	Resource<Shader> shaderBack
	{
		"Shader/Unlit.hlsl",
		RasterizerConfig
		{
			.cullMode = D3D11_CULL_BACK,
			.fillMode = D3D11_FILL_SOLID,
			.frontCounterClockwise = false,
	}
	};  ///< 裏シェーダ
	Resource<Model> model{ "Model/TestAlphaCube/TestAlphaCube.fbx" };

	Component<Name> name{ "TestAlphaCube" };      ///< 名前コンポーネント
	Component<Transform> transform{};      ///< 座標系
	Component<ImTransform> imTransform{};  ///< 座標系表示用

	Component<StaticOBB> obb{};  ///< OBB当たり判定
};

struct TestAlphaCube : ObjectBase<TestAlphaCubeData>
{
	void Start() override;
	void Draw() override;
};
