
#include "TerrainParts.h"

void TerrainParts::Start()
{
	// 地形のオフセット
	transform.position.x = index.x * widthScale;
	transform.position.y = -offsetHeight;
	transform.position.z = index.y * widthScale;
	

	// 地形のサーフェースを作成
	Get<Dx11Manager>().Ref([this](const ComPtr<ID3D11Device>& _pDevice)
	{
		using Vertex = ModelVertex;

		// 横頂点の総数
		const uint32_t n = widthVertexCount;

#pragma region 頂点作成
		std::vector<Vertex> vertices(n * n);

		float dx = widthScale / (n - 1);
		float dz = widthScale / (n - 1);

		for (uint32_t z = 0; z < n; z++)
		{
			for (uint32_t x = 0; x < n; x++)
			{
				uint32_t index = z * n + x;

				// 中心が原点になるように引くオフセット
				const float HALF_OFFSET{ widthScale * 0.0f/*0.5f*/ };

				// 座標を計算
				Vec3 position{};

				// xz座標を求める
				position.x = x * dx - HALF_OFFSET;
				position.z = z * dz - HALF_OFFSET;

				// TODO: パーリンノイズで高さを取得
				position.y = heightMap[z][x] * (maxHeight / UINT16_MAX);

				// 座標系の座標分ずらす
				//position += transform.WorldPosition();

				vertices[index].position = position;

				// UVを計算
				vertices[index].uv = Vec2{ dx * x, dz * z };

				// 法線を計算
				vertices[index].normal = Vec3::Up();  // 後に計算
			}
		}
#pragma endregion

#pragma region インデックス作成
		// 1セルあたり三角形2枚のため6インデックス
		std::vector<uint32_t> indices((n - 1) * (n - 1) * 6);

		int indicesCount = 0;
		for (uint32_t z = 0; z < n - 1; z++)
		{
			for (uint32_t x = 0; x < n - 1; x++)
			{
				// 四角形を構成する4つの頂点インデックス
				uint32_t topLeft = z * n + x;
				uint32_t topRight = z * n + (x + 1);
				uint32_t bottomLeft = (z + 1) * n + x;
				uint32_t bottomRight = (z + 1) * n + (x + 1);

				// 三角形1(時計回り: 左上 -> 右上 -> 左下)
				indices[indicesCount++] = topLeft;
				indices[indicesCount++] = topRight;
				indices[indicesCount++] = bottomLeft;

				// 三角形2(時計回り: 右上 -> 右下 -> 左下)
				indices[indicesCount++] = topRight;
				indices[indicesCount++] = bottomRight;
				indices[indicesCount++] = bottomLeft;
			}
		}
#pragma endregion

#pragma region 法線を整備
		// 全三角形を巡回して、各頂点に面法線を加算していく
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t i0 = indices[i];
			uint32_t i1 = indices[i + 1];
			uint32_t i2 = indices[i + 2];

			Vec3 v0 = vertices[i0].position.v;
			Vec3 v1 = vertices[i1].position.v;
			Vec3 v2 = vertices[i2].position.v;

			// 三角形を形成する2つのベクトルを計算
			Vec3 edge1 = v1 - v0;
			Vec3 edge2 = v2 - v0;

			// 外積によって面法線を求める
			Vec3 faceNormal = DirectX::XMVector3Cross(edge1, edge2);

			// 頂点に法線を蓄積
			Vec3 n0 = Vec3{ vertices[i0].normal.v } + faceNormal;
			Vec3 n1 = Vec3{ vertices[i1].normal.v } + faceNormal;
			Vec3 n2 = Vec3{ vertices[i2].normal.v } + faceNormal;

			vertices[i0].normal = n0;
			vertices[i1].normal = n1;
			vertices[i2].normal = n2;
		}

		// 最後にすべての頂点法線を正規化
		for (uint32_t i = 0; i < n * n; i++)
		{
			Vec3 normal = vertices[i].normal.v;
			normal = DirectX::XMVector3Normalize(normal);
			vertices[i].normal = normal;
		}
#pragma endregion

#pragma region 頂点バッファ作成
		const D3D11_BUFFER_DESC VERTEX_BUFFER_DESC
		{
			.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = NULL,
			.MiscFlags = NULL,
			.StructureByteStride = 0,
		};

		ComPtr<ID3D11Buffer> pVertexBuffer{};

		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA_VERTEX
		{
			.pSysMem = vertices.data(),
			.SysMemPitch = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			.SysMemSlicePitch = 0,
		};

		_pDevice->CreateBuffer(&VERTEX_BUFFER_DESC, &INITIALIZE_DATA_VERTEX, pVertexBuffer.GetAddressOf());
#pragma endregion

#pragma region インデックスバッファ作成

		const D3D11_BUFFER_DESC INDEX_BUFFER_DESC
		{
			.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = NULL,
			.MiscFlags = NULL,
			.StructureByteStride = 0,
		};

		ComPtr<ID3D11Buffer> pIndexBuffer{};

		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA_INDEX
		{
			.pSysMem = indices.data(),
			.SysMemPitch = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			.SysMemSlicePitch = 0,
		};

		_pDevice->CreateBuffer(&INDEX_BUFFER_DESC, &INITIALIZE_DATA_INDEX, pIndexBuffer.GetAddressOf());
#pragma endregion

		SurfaceResource surface
		{
			.pVertexBuffer = pVertexBuffer,
			.pIndexBuffer = pIndexBuffer,
			.size = static_cast<UINT>(sizeof(Vertex)),
			.offset = 0,
			.vertexCount = static_cast<UINT>(vertices.size()),
			.indexCount = static_cast<UINT>(indices.size()),
			.toplogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		};

		this->surface = { std::move(surface) };
	});
}

void TerrainParts::Draw()
{
	Dx11Manager& dx11 = Get<Dx11Manager>();

	dx11.SetMaterial(transform.GetMaterial());
	dx11.SetMaterial(commonResource.material);

	dx11.SetShader(commonResource.shader);
	dx11.SetSurface(surface);
	dx11.SetTexture(commonResource.texture);

	dx11.Draw();
}

float TerrainParts::GetHeight(const float _x, const float _z) const
{
	const float HALF_OFFSET{ 0.0f, /*widthScale * 0.5f*/ };

	// 頂点間の距離xz
	const float DISTANCE_X = widthScale / (widthVertexCount - 1);
	const float DISTANCE_Z = widthScale / (widthVertexCount - 1);

	float x{ (_x + HALF_OFFSET) / DISTANCE_X };
	float z{ (_z + HALF_OFFSET) / DISTANCE_Z };

	// マップの範囲外チェック クランプする
	x = std::clamp(x, 0.0f, static_cast<float>(TERRAIN_MAP_RESOLUTION - 2));
	z = std::clamp(z, 0.0f, static_cast<float>(TERRAIN_MAP_RESOLUTION - 2));


	// 左上の格子点のインデクスを取得
	int gridX = static_cast<int>(std::floor(x));
	int gridZ = static_cast<int>(std::floor(z));

	// 格子内での細かい位置0.0 .. 1.0の小数点以下を取得
	float dx = x - gridX;
	float dz = z - gridZ;

	// 四隅のグリッドの高さを取得
	float h00 = static_cast<float>(heightMap[gridZ][gridX]);          // 左上 (X, Z)
	float h10 = static_cast<float>(heightMap[gridZ][gridX + 1]);      // 右上 (X+1, Z)
	float h01 = static_cast<float>(heightMap[gridZ + 1][gridX]);      // 左下 (X, Z+1)
	float h11 = static_cast<float>(heightMap[gridZ + 1][gridX + 1]);  // 右下 (X+1, Z+1)

	float finalHeight = 0.0f;

	// 斜め線で分けて、2つの三角形のどちらにいるか判定
	if (dx + dz <= 1.0f)
	{
		// 上側の三角形 (h00, h10, h01)
		// 傾きを求めて高さを補間する
		finalHeight = h00 + (h10 - h00) * dx + (h01 - h00) * dz;
	}
	else
	{
		// 下側の三角形 (h10, h11, h01)
		// 右下の点をベースに補間する
		finalHeight = h11 + (h01 - h11) * (1.0f - dx) + (h10 - h11) * (1.0f - dz);
	}

	finalHeight = finalHeight / UINT16_MAX * maxHeight;
	finalHeight += transform.position.y;

	VCU_LOG("{} height:{}", (std::string_view)name, finalHeight);

	return finalHeight;
}

Vec3 TerrainParts::GetNormal(const float _x, const float _z) const
{
	const float HALF_OFFSET{ 0.0f /*widthScale * 0.5f*/ };

	// 頂点間の距離xz
	const float DISTANCE_X = widthScale / (widthVertexCount - 1);
	const float DISTANCE_Z = widthScale / (widthVertexCount - 1);

	float x{ (_x + HALF_OFFSET) / DISTANCE_X };
	float z{ (_z + HALF_OFFSET) / DISTANCE_Z };

	// マップの範囲外チェック クランプする（インデックス参照のため+1を考慮して制限）
	x = std::clamp(x, 0.0f, static_cast<float>(TERRAIN_MAP_RESOLUTION - 2));
	z = std::clamp(z, 0.0f, static_cast<float>(TERRAIN_MAP_RESOLUTION - 2));

	// 左上の格子点のインデクスを取得
	int gridX = static_cast<int>(std::floor(x));
	int gridZ = static_cast<int>(std::floor(z));

	// 格子内での細かい位置0.0 .. 1.0の小数点以下を取得
	float dx = x - gridX;
	float dz = z - gridZ;

	// 四隅のグリッドの高さを現実のスケール（メートル単位）に変換して取得
	const float factor = static_cast<float>(maxHeight) / static_cast<float>(UINT16_MAX);
	float h00 = static_cast<float>(heightMap[gridZ][gridX]) * factor;          // 左上
	float h10 = static_cast<float>(heightMap[gridZ][gridX + 1]) * factor;      // 右上
	float h01 = static_cast<float>(heightMap[gridZ + 1][gridX]) * factor;      // 左下
	float h11 = static_cast<float>(heightMap[gridZ + 1][gridX + 1]) * factor;  // 右下

	DirectX::XMVECTOR normalVector;

	// 斜め線で分けて、2つの三角形のどちらにいるか判定
	if (dx + dz <= 1.0f)
	{
		// 上側の三角形 (h00, h10, h01)
		// 頂点情報から正確な2つのエッジベクトルを作成（実際の距離 DISTANCE_X / Z を考慮）
		Vec3 edgeX{ DISTANCE_X, h10 - h00, 0.0f }; // 左上から右上へのベクトル
		Vec3 edgeZ{ 0.0f, h01 - h00, DISTANCE_Z }; // 左上から左下へのベクトル

		// 【修正】左手系ルールに基づき、Z軸ベクトル × X軸ベクトルの順で外積を計算
		normalVector = DirectX::XMVector3Cross(edgeZ, edgeX);
	}
	else
	{
		// 下側の三角形 (h10, h11, h01)
		Vec3 edgeX{ -DISTANCE_X, h01 - h11, 0.0f }; // 右下から左下へのベクトル
		Vec3 edgeZ{ 0.0f, h10 - h11, -DISTANCE_Z }; // 右下から右上へのベクトル

		// 【修正】外積計算
		normalVector = DirectX::XMVector3Cross(edgeZ, edgeX);
	}

	// しっかり正規化して返す（これで長さ1の純粋な方向ベクトルになる）
	return DirectX::XMVector3Normalize(normalVector);
}

