// 地形のマテリアル
cbuffer SpriteRendererMaterial : register(b0)
{
    int4 SpriteRendererMaterial_HasTexture; // テクスチャを持っているか
    float4 SpriteRendererMaterial_BaseColor; // ベースの色
}
