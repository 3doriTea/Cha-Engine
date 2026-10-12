// モデルのマテリアル
cbuffer ModelMaterial : register(b0)
{
	row_major matrix ModelMaterial_World;
	int4 ModelMaterial_HasTexture_HasSkin_HasAnim;
	float4 ModelMaterial_BaseColor;
}
