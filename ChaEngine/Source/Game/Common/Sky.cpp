
#include "Sky.h"
#include "Sky/SkyMaterial.h"
#include "../System/CameraSystem.h"

void Sky::Start()
{
	Get<Dx11Manager>().Ref([this](const ComPtr<ID3D11Device>& _pDevice)
	{
		struct alignas(16) Vertex
		{
			Float4 position;
		};

#pragma region 頂点バッファ作成
		static std::array<Vertex, VERTEX_COUNT> vertices
		{
			// 前の面 (Z = +0.5f)
			Vertex{ .position = Float4{  0.5f,  0.5f, 0.5f, 1.0f } }, // 右上
			Vertex{ .position = Float4{  0.5f, -0.5f, 0.5f, 1.0f } }, // 右下
			Vertex{ .position = Float4{ -0.5f, -0.5f, 0.5f, 1.0f } }, // 左下
			Vertex{ .position = Float4{ -0.5f,  0.5f, 0.5f, 1.0f } }, // 左上

			// 後ろの面 (Z = -0.5f)
			Vertex{ .position = Float4{ -0.5f,  0.5f, -0.5f, 1.0f } }, // 左上
			Vertex{ .position = Float4{  0.5f,  0.5f, -0.5f, 1.0f } }, // 右上
			Vertex{ .position = Float4{  0.5f, -0.5f, -0.5f, 1.0f } }, // 右下
			Vertex{ .position = Float4{ -0.5f, -0.5f, -0.5f, 1.0f } }, // 左下
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
		static std::array<uint32_t, INDEX_COUNT> indices
		{
			// 前の面 (内側から見て手前)
			0, 1, 2, 0, 2, 3,
			// 後ろの面 (内側から見て奥)
			5, 4, 7, 5, 7, 6,
			// 左の面
			4, 3, 2, 4, 2, 7,
			// 右の面
			0, 5, 6, 0, 6, 1,
			// 上の面
			0, 3, 4, 0, 4, 5,
			// 下の面
			2, 1, 6, 2, 6, 7,
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

		skyBoxSurface = { std::move(surface) };
	});
	{  // マテリアル作成
		MaterialResource material{};

		std::unique_ptr<SkyMaterialProperty> pModelProperty = std::make_unique<SkyMaterialProperty>();
		pModelProperty->Get().transform = Mat4x4::Identity();

		material.TryLoad(std::move(pModelProperty));

		this->material = Resource<ResourceType::Material>{ std::move(material) };
	}
}

void Sky::Update()
{
	//ImGui::Begin("Sky");

	//ImGui::DragFloat4("time", skyTime.v, 0.001f, 0.0f, 1.0f);

	//ImGui::End();
}

void Sky::Draw()
{
	CameraSystem& cameraSystem = Get<CameraSystem>();

	// スカイボックス用のビュー行列
	Mat4x4 viewMatrix{ cameraSystem.Data().viewMatrix };
	viewMatrix.r[3] = { 0, 0, 0, 1.0f };  // 平行移動無視

	Mat4x4 projection{ cameraSystem.Data().projectionMatrix };

	SkyMaterialProperty* pSkyProperty = static_cast<SkyMaterialProperty*>(material.Get().pProperty.get());
	pSkyProperty->Get().transform = (Mat4x4::Identity(), viewMatrix* projection);
	pSkyProperty->Get().time = skyTime;

	Get<Dx11Manager>().ChangeDepthState(DepthStateType_Background);

	Get<Dx11Manager>().SetMaterial(material);

	Get<Dx11Manager>().SetShader(skyBoxShader);
	Get<Dx11Manager>().SetTexture(skyboxTextureSpace, 0);
	Get<Dx11Manager>().SetTexture(skyboxTextureDay, 1);

	Get<Dx11Manager>().SetSurface(skyBoxSurface);

	Get<Dx11Manager>().Draw(INDEX_COUNT);

	Get<Dx11Manager>().ChangeDepthState(DepthStateType_Standard);
}
