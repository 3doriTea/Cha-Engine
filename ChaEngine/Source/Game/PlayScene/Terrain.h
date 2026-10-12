#pragma once
#include <Game/Game.h>
#include "TerrainMapResolution.h"
#include "TerrainMapChunk.h"
#include "Terrain/TerrainCommonResource.h"

namespace std
{
	template <>
	struct hash<cha::Vec2Int>
	{
		std::size_t operator()(const cha::Vec2Int& v) const noexcept
		{
			// X と Y のハッシュ値を計算
			std::size_t h1 = std::hash<int>()(v.x);
			std::size_t h2 = std::hash<int>()(v.y);

			return h1 ^ (h2 << 1);
		}
	};
}

/// @brief 地形のデータ
struct TerrainData
{
	Resource<Texture> map{ "TerrainImage/-0_0.png" };
	//float widthScale{ 100.0f };  ///< 1辺のサイズ
	float widthScale{ 10.0f };  ///< 1辺のサイズ
	//float maxHeight{ 600.0f };   ///< 地形の最大の高さ
	float maxHeight{ 60.0f };   ///< 地形の最大の高さ

	/// @brief uint8_tの色
	/// @memo cha::ColorはRGBAを浮動小数点で持つためサイズが大きくなる
	struct ColorUInt8
	{
		uint8_t r{};
		uint8_t g{};
		uint8_t b{};
		uint8_t a{};
	};

	std::unordered_map<Vec2Int, GameObject> loadObjTerrainParts{};  ///< 読み込んだ地形のパーツ

	TerrainCommonResource commonResource{};  ///< 地形共通のリソース
};

/// @brief 地形を統括するオブジェクト
struct Terrain : ObjectBase<TerrainData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 地形の高さを取得する
	/// @param _x x座標
	/// @param _z z座標
	/// @return 地形の高さ y座標
	float GetHeight(const float _x, const float _z);

	/// @brief 地形の法線を取得する
	/// @param _x x座標
	/// @param _z z座標
	/// @return 地形の法線
	Vec3 GetNormal(const float _x, const float _z);

private:
	/// @brief 共通のリソースの読み込み処理
	void LoadCommonResource();

	/// @brief 画像から地形を読み込む
	/// @param _terrainImage 画像のファイルパス
	/// @param _metadata 出力されるメタデータ
	/// @param _image 出力される画像データ
	void LoadTerrainImage(const fs::path& _terrainImage,
		DirectX::TexMetadata& _metadata,
		DirectX::ScratchImage& _image);

	/// @brief 画像のデータから高さ情報を読み取る
	/// @param _image 画像データ
	/// @param _writeMap 高さデータの書き込み先
	/// @param _offsetPixel 切り取るピクセルオフセット
	/// @param _sizePixel 切り取るサイズ
	void LoadTerrainHeights(
		const DirectX::ScratchImage& _image,
		TerrainMapChunk& _writeMap,
		Vec2Int _offsetPixel,
		Vec2Int _sizePixel);

	uint16_t ImageColorToHeight(const ColorUInt8 _color);
};
