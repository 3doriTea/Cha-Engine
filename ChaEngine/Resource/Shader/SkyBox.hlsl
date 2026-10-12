#include "Vertex/SkyVertex.hlsli"
#include "Material/SkyMaterial.hlsli"
#include "Camera.hlsli"

// 宇宙のテクスチャ
TextureCube g_SpaceTexture : register(t0);
SamplerState g_SpaceSampler : register(s0);

// 日中のテクスチャ
TextureCube  g_DayTexture : register(t1);
SamplerState g_DaySampler : register(s1);

struct VSOutput
{
	float4 pos : SV_Position;    // 頂点座標
	float3 texCoord : TEXCOORD0; // サンプル座標
};

VSOutput VS(SkyVertex input)
{
	VSOutput output;
	output.texCoord = input.pos.xyz;

	// 平行移動成分を消したView行列とプロジェクション行列と回転が入っているSkyMaterial_Transform
	float4 pos = mul(input.pos, SkyMaterial_Transform);

	pos.z = pos.w; // 一番奥になる
	output.pos = pos.xyzw;

	return output;
}

float4 PS(VSOutput input) : SV_Target0
{
	float4 color = float4(0, 0, 0, 1);

	color += g_SpaceTexture.Sample(g_SpaceSampler, input.texCoord) * SkyMaterial_Time.x * SkyMaterial_Time.y;
	color += g_DayTexture.Sample(g_DaySampler, input.texCoord) * (1.0f - SkyMaterial_Time.x);
	color += float4(SkyMaterial_Time.z, 0.0f, 0.0f, 0.0f);
	color += float4(SkyMaterial_Time.w, SkyMaterial_Time.w, 0.0f, 0.0f);

	return color;
}
