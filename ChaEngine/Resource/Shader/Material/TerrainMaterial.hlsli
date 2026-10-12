// 地形のマテリアル
cbuffer TerrainMaterial : register(b0)
{
	int4 TerrainMaterial_HasTexture;
	float4 TerrainMaterial_BaseColor;
}
