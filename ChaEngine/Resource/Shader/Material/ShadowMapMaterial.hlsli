// シャドウマップ用マテリアル
cbuffer ShadowMapMaterial : register(b0)
{
	row_major matrix ShadowMapMaterial_Transform;
};
