#include "Camera.hlsli"
#include "Material/DebugOBBMaterial.hlsli"
#include "Vertex/ModelVertex.hlsli"

struct VSOutput
{
	float4 pos : SV_Position; // 頂点座標
	float2 uv : TEXCOORD0; // UV座標
};

VSOutput VS(ModelVertex input)
{
	VSOutput output;
	float4 worldPos = input.pos;
	worldPos = mul(worldPos, DebugOBBMaterial_Transform);
	
	output.pos = mul(worldPos, Camera_ViewProjection);
	
	output.uv = input.uv;
	return output;
}

float4 PS(VSOutput input) : SV_Target0
{
	float4 diffuseColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	return diffuseColor;
}
