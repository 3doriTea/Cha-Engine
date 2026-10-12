#include "Material/LightMaterial.hlsli"
#include "ModelTransform.hlsli"
#include "Transform.hlsli"
#include "Vertex/ModelVertex.hlsli"


struct VSOutput
{
	float4 pos : SV_Position; // 頂点座標
};

VSOutput VS(ModelVertex input)
{
	VSOutput output;
	float4 worldPos = input.pos;
	worldPos = mul(worldPos, ModelTransform_ToWorldMatrix);
	worldPos = mul(worldPos, Transform_World);
	
	output.pos = mul(worldPos, LightMaterial_ViewProjection);

	return output;
}

void PS(VSOutput input)
{
	// depth stencil view で自動で深さは見つけてくれるから何もせんでいい
}
