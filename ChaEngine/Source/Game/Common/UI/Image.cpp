#include "Image.h"

void UI::Image::Start()
{
	Get<Dx11Manager>().Ref([this](const ComPtr<ID3D11Device>& _pDevice)
	{
		struct alignas(16) Vertex
		{
			Float4 position;
			Float2 uv;
		};

#pragma region 頂点バッファ作成
		static std::array<Vertex, 4> vertices
		{
			Vertex{ .position = Vec3{ -0.5f, -0.5f, 0.0f }, .uv = Vec2{ 0, 1 } },
			Vertex{ .position = Vec3{ -0.5f, 0.5f, 0.0f }, .uv = Vec2{ 0, 0 } },
			Vertex{ .position = Vec3{ 0.5f, -0.5f, 0.0f }, .uv = Vec2{ 1, 1 } },
			Vertex{ .position = Vec3{ 0.5f, 0.5f, 0.0f }, .uv = Vec2{ 1, 0 } },
		};

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
		static std::array<uint32_t, 6> indices
		{
			0, 1, 2,
			1, 3, 2
		};

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

		triangleSuraface = { std::move(surface) };
	});
}

void UI::Image::Draw()
{
	Get<Dx11Manager>().SetShader(simpleShader);
	Get<Dx11Manager>().SetSurface(triangleSuraface);
	Get<Dx11Manager>().SetTexture(texture);
	Get<Dx11Manager>().Draw();
}
