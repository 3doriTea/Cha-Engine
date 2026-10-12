
#include "TestAlphaCube.h"

void TestAlphaCube::Start()
{
	transform.scale = Vec3::One() * 100.0f;
	obb.scale = Vec3::One() * 0.001f;
}

void TestAlphaCube::Draw()
{
	Get<Dx11Manager>().EnqueueTransparent([this]
	{
		// ブレンドステートは透明度考慮するやつ
		Get<Dx11Manager>().SetBlendState(BlendStateType_Alpha);
		// 深度バッファは書き込まない
		Get<Dx11Manager>().ChangeDepthState(DepthStateType_Background);

		// Transformをコンスタントバッファに送る
		Get<Dx11Manager>().SetMaterial(transform.GetMaterial());
		
		// 各モデルのパーツを取ってくる
		const auto& subParts{ model.Get().subParts };
		// 各モデルのパーツを描画しまくる
		auto DrawParts = [this, subParts]
		{
			for (const auto& parts : subParts)
			{
				Get<Dx11Manager>().SetSurface(parts.mesh);
				Get<Dx11Manager>().SetMaterial(parts.material);
				Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
				Get<Dx11Manager>().Draw();
			}
		};

		// 表描画
		Get<Dx11Manager>().SetShader(shaderFront);
		DrawParts();

		// 裏描画
		Get<Dx11Manager>().SetShader(shaderBack);
		DrawParts();

		// 深度バッファもブレンドモードも元に戻す
		Get<Dx11Manager>().ChangeDepthState(DepthStateType_Standard);
		Get<Dx11Manager>().SetBlendState(BlendStateType_None);
	});
}
