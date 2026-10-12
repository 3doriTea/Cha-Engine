#pragma once
#include <Game/Game.h>
#include "TerrainMapResolution.h"
#include "TerrainMapChunk.h"
#include "Terrain/TerrainCommonResource.h"

/// @brief 地形のチャンクパーツのデータ
struct TerrainPartsData
{

	/// @brief 地形チャンクパーツのデータを初期化する
	/// @param _index 地形のインデクス
	/// @param _widthScale 地形のスケール
	/// @param _maxHeight 地形最大の高さ
	/// @param _heightMap 地形の高さマップデータ
	TerrainPartsData(
		const Vec2Int _index,
		const float _widthScale,
		const float _maxHeight,
		TerrainMapChunk&& _heightMap,
		TerrainCommonResource& _commonResource) :
		index{ _index },
		widthScale{ _widthScale },
		maxHeight{ _maxHeight },
		offsetHeight{ _maxHeight * 0.5f },
		heightMap{ std::move(_heightMap) },
		commonResource{ _commonResource },
		name{ std::format("TerrainParts({},{})", _index.x, _index.y) }
	{}

	Vec2Int index{};  ///< 地形の座標オフセット

	TerrainMapChunk heightMap{};  ///< 高さマップ

	Component<Name> name{};  ///< オブジェクトの名前

	uint32_t widthVertexCount{ TERRAIN_MAP_VERTICES_WIDTH };  ///< 1辺の頂点数
	float widthScale{};                ///< 1辺のサイズ
	float maxHeight{};                 ///< 地形の最大の高さ
	float offsetHeight{};              ///< 高さのオフセット
	Resource<Surface> surface{};       ///< 地形の面
	Component<Transform> transform{};  ///< 座標系
	TerrainCommonResource& commonResource;  ///< 地形共通のリソース
};

/// @brief 地形のチャンクパーツ
struct TerrainParts : ObjectBase<TerrainPartsData>
{
	using ObjectBase<TerrainPartsData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 地形の高さを取得する
	/// @param _x x座標
	/// @param _z z座標
	/// @return 地形の高さ y座標
	float GetHeight(const float _x, const float _z) const;

	/// @brief 地形の法線を取得する
	/// @param _x x座標
	/// @param _z z座標
	/// @return 地形の法線
	Vec3 GetNormal(const float _x, const float _z) const;
};
