#include "Material/TerrainMaterial.hlsli"
#include "Camera.hlsli"
#include "Transform.hlsli"
#include "Vertex/ModelVertex.hlsli"
#include "Material/LightMaterial.hlsli"
#include "ShadowMapTexture.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VSOutput
{
	float4 pos : SV_Position; // 頂点座標
	float2 uv : TEXCOORD0; // UV座標
	float4 lightSpacePos : TEXCOORD1; // ライトから見た頂点座標
	float3 worldPos : POSITION; // 【追加】フォグ計算用のワールド座標
};

VSOutput VS(ModelVertex input)
{
	VSOutput output;
	float4 worldPos = input.pos;
	worldPos = mul(worldPos, Transform_World);
	
	output.pos = mul(worldPos, Camera_ViewProjection);
	output.uv = input.uv;
	output.lightSpacePos = mul(worldPos, LightMaterial_ViewProjection);
	
	// 【追加】ピクセルシェーダーにワールド座標を渡す
	output.worldPos = worldPos.xyz;
	
	return output;
}

float4 hash4fast(float2 p)
{
	float4 dot_results = float4(
		dot(p, float2(127.1f, 311.7f)),
		dot(p, float2(269.5f, 183.3f)),
		dot(p, float2(419.2f, 371.9f)),
		dot(p, float2(321.4f, 245.6f))
	);
	return frac(sin(dot_results) * 43758.5453123f);
}

float4 PS(VSOutput input) : SV_Target0
{
	// --- 元のテクスチャ・タイリング処理 ---
	float4 off = hash4fast(floor(input.uv));
	off.zw = off.zw >= float2(0.5f, 0.5f) ? float2(1.0f, 1.0f) : float2(-1.0f, -1.0f);
	float2 fuv = frac(input.uv);
	float2 uv = fuv * off.zw + off.xy;
	
	float4 diffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (TerrainMaterial_HasTexture.x)
	{
		diffuseColor = g_texture.Sample(g_sampler, uv);
	}
	else
	{
		diffuseColor = TerrainMaterial_BaseColor;
	}
	
	// --- シャドウマップ処理 ---
	float2 shadowUV = input.lightSpacePos.xy / input.lightSpacePos.w;
	shadowUV.x = shadowUV.x * 0.5f + 0.5f;
	shadowUV.y = -shadowUV.y * 0.5f + 0.5f;
	
	float currentDepth = input.lightSpacePos.z / input.lightSpacePos.w;
	float bias = 0.0005f;
	float shadowMultiplier = 1.0f;
	
	if (shadowUV.x >= 0.0f && shadowUV.x <= 1.0f && shadowUV.y >= 0.0f && shadowUV.y <= 1.0f)
	{
		float isLit = g_shadowMap.SampleCmpLevelZero(g_shadowSampler, shadowUV, currentDepth - bias).r;
		if (isLit < 0.5f)
		{
			shadowMultiplier = 0.4f;
		}
	}
	
	// 影を適用した最終的な地形のベース色
	float3 finalColor = diffuseColor.rgb * shadowMultiplier;

	// ==========================================
	// 【追加】距離ベースのフォグ（薄くする処理）
	// ==========================================
	
	// 1. カメラとピクセルの世界座標の距離を計算
	// ※Camera_PositionはCamera.hlsli内で定義されているカメラの座標変数名に合わせて適宜変更してください
	float dist = distance(Camera_Position.xyz, input.worldPos);
	
	// 2. フォグの開始距離と終了距離を設定 (ゲームの規模に合わせて数値を調整してください)
	float fogStart = 10.0f; // これより手前はくっきり見える
	float fogEnd = 100.0f; // これより奥は完全にフォグの色になる
	
	// 3. 距離に応じたフォグの濃さ (0.0 ～ 1.0) を計算
	float fogFactor = saturate((dist - fogStart) / (fogEnd - fogStart));
	
	// 4. 遠くを「薄く」するための色（背景の空の色や、薄いグレーなど）
	float3 fogColor = float3(0.6f, 0.7f, 0.8f); // 例：淡い水色/グレー
	
	// 5. 元の色とフォグの色を線形補間する
	finalColor = lerp(finalColor, fogColor, fogFactor);
	
	// ==========================================

	return float4(finalColor, diffuseColor.a);
}
