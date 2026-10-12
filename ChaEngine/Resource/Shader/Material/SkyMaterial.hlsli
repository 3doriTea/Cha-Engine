// 空のマテリアル
cbuffer SkyMaterial : register(b0)
{
	row_major matrix SkyMaterial_Transform;  // 座標変換(平行移動なしView * Projection * 回転)
	float4 SkyMaterial_Time;  // 各軸の時間
}
