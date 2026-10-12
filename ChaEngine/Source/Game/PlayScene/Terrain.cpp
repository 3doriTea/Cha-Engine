
#include "Terrain.h"
#include "TerrainParts.h"
#include <Game/Common/Terrain/TerrainMaterial.h>

void Terrain::Start()
{
	LoadCommonResource();

	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage image{};
	LoadTerrainImage("TerrainImage/-0_0.png", metadata, image);

	// imageを取得する
	const DirectX::Image* pImage = image.GetImage(0, 0, 0);
	VCU_ASSERT(pImage && "画像の取得に失敗");
	if (!pImage)
	{
		return;  // イメージの取得に失敗
	}

	// 画像から読み取れるチャンク数
	const Vec2Int CHUNK_COUNT = Vec2Int{ pImage->width, pImage->height } / TERRAIN_MAP_RESOLUTION;

	Vec2Int chunkIndex{};
	for (chunkIndex.y = 0; chunkIndex.y < CHUNK_COUNT.y; chunkIndex.y++)
	{
		for (chunkIndex.x = 0; chunkIndex.x < CHUNK_COUNT.x; chunkIndex.x++)
		{
			TerrainMapChunk mapChunk{};

			Vec2Int size{ Vec2Int::One() * TERRAIN_MAP_VERTICES_WIDTH };

			if (chunkIndex.x >= CHUNK_COUNT.x - 1)
			{
				// xが端っこなら範囲外アクセスを避けるために1つ減らす
				size.x -= 1;
			}

			if (chunkIndex.y >= CHUNK_COUNT.y - 1)
			{
				// yが端っこなら範囲外アクセスを避けるために1つ減らす
				size.y -= 1;
			}

			VCU_LOG("生成チャンク: {}, {}", chunkIndex.x, chunkIndex.y);

			LoadTerrainHeights(
				image,
				mapChunk,
				chunkIndex * TERRAIN_MAP_RESOLUTION,
				size);

			//bool showFlag{ false };
			//std::stringstream ss{};

			//for (size_t y = 0; y < mapChunk.size(); y++)
			//{
			//	for (int x = 0; x < mapChunk[y].size(); x++)
			//	{
			//		ss << mapChunk[y][x] << ", ";
			//		if (mapChunk[y][x] != 0)
			//		{
			//			showFlag = true;  // 1つでも0以外があったらその地形は表示したい
			//		}
			//	}
			//	ss << std::endl;
			//}

			//if (showFlag)  // 表示する場合はlog出力
			//{
			//	VCU_LOG("{}", ss.str());
			//}

			GameObject objTerrainParts = GetScene().Instantiate<TerrainParts>(
				chunkIndex,
				widthScale,
				maxHeight,
				std::move(mapChunk),
				commonResource);

			// 読み込んだ地形を記憶しておく
			loadObjTerrainParts.emplace(chunkIndex, objTerrainParts);

			//break;
			//return;
		}
	}
}

void Terrain::Update()
{
}

float Terrain::GetHeight(const float _x, const float _z)
{
	Vec2Int index
	{
		static_cast<int>(std::floor(_x / widthScale)),
		static_cast<int>(std::floor(_z / widthScale))
	};

	auto itr = loadObjTerrainParts.find(index);
	if (itr == loadObjTerrainParts.end())
	{
		return 0.0f;
	}

	TerrainParts& terrainParts = Get<TerrainParts>(itr->second);
	return terrainParts.GetHeight(
		_x - (index.x * widthScale),
		_z - (index.y * widthScale));
}

Vec3 Terrain::GetNormal(const float _x, const float _z)
{
	Vec2Int index
	{
		static_cast<int>(std::floor(_x / widthScale)),
		static_cast<int>(std::floor(_z / widthScale))
	};

	auto itr = loadObjTerrainParts.find(index);

	if (itr == loadObjTerrainParts.end())
	{
		return Vec3::Up();
	}

	TerrainParts& terrainParts = Get<TerrainParts>(itr->second);
	return terrainParts.GetNormal(
		_x - (index.x * widthScale),
		_z - (index.y * widthScale));
}

void Terrain::LoadCommonResource()
{
	{  // マテリアル作成
		MaterialResource material{};

		std::unique_ptr<TerrainMaterialProperty> pModelProperty = std::make_unique<TerrainMaterialProperty>();
		pModelProperty->Get().hasTexture = { TRUE, TRUE, TRUE, TRUE };
		pModelProperty->Get().baseColor = { 1, 1, 1, 1 };

		material.TryLoad(std::move(pModelProperty));

		this->commonResource.material = Resource<Material>{ std::move(material) };
	}
}

void Terrain::LoadTerrainImage(
	const fs::path& _terrainImage,
	DirectX::TexMetadata& _metadata,
	DirectX::ScratchImage& _image)
{
	using namespace DirectX;

	HRESULT hResult{};
	// WICファイルから読み込む
	hResult = LoadFromWICFile(
		_terrainImage.c_str(),
		WIC_FLAGS_NONE,
		&_metadata,
		_image);

	VCU_ASSERT(SUCCEEDED(hResult) && "地形画像の読み込みに失敗");

	// フォーマットが透過32bit PNG ではないなら強制的に変換
	if (_metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		ScratchImage converted{};
		hResult = Convert(_image.GetImages(), _image.GetImageCount(), _metadata,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			TEX_FILTER_DEFAULT,
			TEX_THRESHOLD_DEFAULT,
			converted);

		VCU_ASSERT(SUCCEEDED(hResult) && "地形画像の透過png変換に失敗");
	}
}

void Terrain::LoadTerrainHeights(
	const DirectX::ScratchImage& _image,
	TerrainMapChunk& _writeMap,
	Vec2Int _offsetPixel,
	Vec2Int _sizePixel)
{
	using namespace DirectX;

	// imageを取得する
	const Image* pImage = _image.GetImage(0, 0, 0);
	VCU_ASSERT(pImage && "画像の取得に失敗");
	if (!pImage)
	{
		return;  // イメージの取得に失敗
	}

	const uint8_t* pixels = pImage->pixels;

	Vec2Int index{};
	for (index.y = 0; index.y < _sizePixel.y; index.y++)
	{
		for (index.x = 0; index.x < _sizePixel.x; index.x++)
		{
			Vec2Int pickPos{ index + _offsetPixel };

			size_t byteOffset = (pickPos.y * pImage->rowPitch) + (pickPos.x * 4);  // 色の4byte分

			enum
			{
				A, R, G, B,
			};
			
			const ColorUInt8 PICK_COLOR
			{
				.r = pixels[byteOffset + R],
				.g = pixels[byteOffset + G],
				.b = pixels[byteOffset + B],
				.a = pixels[byteOffset + A],
			};

			// 地形マップの上の色をマップの高さに変換して書き込む
			_writeMap[index.y][index.x] = ImageColorToHeight(PICK_COLOR);
		}
	}
}

uint16_t Terrain::ImageColorToHeight(const ColorUInt8 _color)
{
	// uint8_tをuint16_tに拡張するレート
	static constexpr int U8TOU16_RATIO{ UINT16_MAX / UINT8_MAX };

	VCU_ASSERT((_color.r <= UINT16_MAX)
		&& "色の範囲オーバー");

	return static_cast<uint16_t>(_color.r * U8TOU16_RATIO);
}
