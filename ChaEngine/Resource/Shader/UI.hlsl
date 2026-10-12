#include "UITransform.hlsli"
#include "Material/SpriteRendererMaterial.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VSInput
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VSOutput
{
	float4 pos : SV_Position; // 頂点座標
	float2 uv : TEXCOORD0; // UV座標
};

VSOutput VS(VSInput input)
{
	VSOutput output;
	float4 pos = float4(input.pos.xyz, 1.0f);

	pos = mul(pos, UITransform_NDC);
	
	output.pos = pos;
	//output.pos.w = 0.0f;
	output.uv = input.uv;
	return output;
}

float4 PS(VSOutput input) : SV_Target0
{
	float4 diffuse;
	if (SpriteRendererMaterial_HasTexture.x)
	{
		diffuse = g_texture.Sample(g_sampler, input.uv);
		// テクスチャがある場合は baseColor で乗算してアルファや色を調整する
		diffuse.rgb *= SpriteRendererMaterial_BaseColor.rgb;
		diffuse.a *= SpriteRendererMaterial_BaseColor.a;
	}
	else
	{
		diffuse = SpriteRendererMaterial_BaseColor;
	}
	return diffuse;
}
