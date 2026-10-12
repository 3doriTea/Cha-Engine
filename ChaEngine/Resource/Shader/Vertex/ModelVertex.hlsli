// モデルの頂点
struct ModelVertex
{
	float4 pos : POSITION; // 頂点座標
	float4 normal : NORMAL; // 法線
	float2 uv : TEXCOORD0; // UV座標
	
	uint4 boneIndices : BONE_INDICES; // モデル用
	float4 boneWeights : BONE_WEIGHTS; // モデル用
};
