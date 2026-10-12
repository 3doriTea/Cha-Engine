
#include "OBBRenderSystem.h"
#include "../Component/Transform.h"
#include "../Component/OBBRenderer.h"
#include "../Component/DynamicOBB.h"
#include "OBBRenderSystem/DebugOBBMaterial.h"

void OBBRenderSystem::Start()
{
	debugOBBMaterial = MaterialResource{ 0 };
	debugOBBMaterial.Get().SetProperty<DebugOBBMaterialProperty>();

	Get<Dx11Manager>().Ref([this](const ComPtr<ID3D11Device>& _pDevice)
	{
		struct alignas(16) Vertex
		{
			Float4 position;
		};

#pragma region 頂点バッファ作成
		static std::array<Vertex, 8> vertices
		{
			Vertex{ .position = Vec3{ -1.0f, 1.0f, -1.0f } },  // 0: 前面上左
			Vertex{ .position = Vec3{ 1.0f, 1.0f, -1.0f } },   // 1: 前面上右
			Vertex{ .position = Vec3{ 1.0f, -1.0f, -1.0f } },  // 2: 前面下右
			Vertex{ .position = Vec3{ -1.0f, -1.0f, -1.0f } }, // 3: 前面下左
			Vertex{ .position = Vec3{ -1.0f, 1.0f, 1.0f } },   // 4: 背面上左
			Vertex{ .position = Vec3{ 1.0f, 1.0f, 1.0f } },    // 5: 背面上右
			Vertex{ .position = Vec3{ 1.0f, -1.0f, 1.0f } },   // 6: 背面下右
			Vertex{ .position = Vec3{ -1.0f, -1.0f, 1.0f } },  // 7: 背面下左
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
		static std::array<uint32_t, 36> indices
		{
			// 前面 (Front面: Z-)
			0, 1, 2,
			0, 2, 3,

			// 背面 (Back面: Z+)
			5, 4, 7,
			5, 7, 6,

			// 上面 (Top面: Y+)
			4, 5, 1,
			4, 1, 0,

			// 下面 (Bottom面: Y-)
			3, 2, 6,
			3, 6, 7,

			// 左側面 (Left面: X-)
			4, 0, 3,
			4, 3, 7,

			// 右側面 (Right面: X+)
			1, 5, 6,
			1, 6, 2
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

		cubeSuraface = { std::move(surface) };
	});
}

void OBBRenderSystem::Draw()
{
	Get<Dx11Manager>().SetShader(debugLineShader);

	auto viewStatic = GetView<OBBRenderer, Transform, StaticOBB>();
	auto viewDynamic = GetView<OBBRenderer, Transform, DynamicOBB>();

	MaterialResource& material = debugOBBMaterial.Get();

	viewStatic.ForEach([this, &material]
	(IObject& _object, OBBRenderer& _renderer, Transform& _transform, StaticOBB& _obb)
	{
		DebugOBBMaterialProperty* pDebugOBBMaterialProperty =
			static_cast<DebugOBBMaterialProperty*>(material.pProperty.get());

		Mat4x4 obbScale = DirectX::XMMatrixScaling(_obb.scale.v[0], _obb.scale.v[1], _obb.scale.v[2]);
		Mat4x4 obbTranslation = DirectX::XMMatrixTranslation(_obb.offset.v[0], _obb.offset.v[1], _obb.offset.v[2]);

		Mat4x4 localWorld = XMMatrixMultiply(obbScale, obbTranslation);
		(*pDebugOBBMaterialProperty)->transform = XMMatrixMultiply(localWorld, _transform.world);

		Get<Dx11Manager>().SetMaterial(debugOBBMaterial);
		Get<Dx11Manager>().SetSurface(cubeSuraface);
		Get<Dx11Manager>().Draw();
	});

	viewDynamic.ForEach([this, &material]
	(IObject& _object, OBBRenderer& _renderer, Transform& _transform, DynamicOBB& _obb)
	{
		DebugOBBMaterialProperty* pDebugOBBMaterialProperty =
			static_cast<DebugOBBMaterialProperty*>(material.pProperty.get());

		Mat4x4 obbScale = DirectX::XMMatrixScaling(_obb.scale.v[0], _obb.scale.v[1], _obb.scale.v[2]);
		Mat4x4 obbTranslation = DirectX::XMMatrixTranslation(_obb.offset.v[0], _obb.offset.v[1], _obb.offset.v[2]);

		Mat4x4 localWorld = XMMatrixMultiply(obbScale, obbTranslation);
		(*pDebugOBBMaterialProperty)->transform = XMMatrixMultiply(localWorld, _transform.world);

		Get<Dx11Manager>().SetMaterial(debugOBBMaterial);
		Get<Dx11Manager>().SetSurface(cubeSuraface);
		Get<Dx11Manager>().Draw();
	});
}
