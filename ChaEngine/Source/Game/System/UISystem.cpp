
#include "UISystem.h"
#include <Base/Manager/UIManager.h>
#include <Base/Manager/D2DManager.h>
#include "../Component/UI/UITransform.h"
#include "../Component/UI/UIImage.h"
#include "../Component/UI/UIText.h"
#include "../Component/UI/UIRaycastTarget.h"
#include "UISystem/SpriteRendererMaterial.h"

void UISystem::Start()
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
			Vertex{ .position = Vec3{ 0.0f, 0.0f, 0.0f }, .uv = Vec2{ 0, 0 } },  // 左上
			Vertex{ .position = Vec3{ 1.0f, 0.0f, 0.0f }, .uv = Vec2{ 1, 0 } },  // 右上
			Vertex{ .position = Vec3{ 0.0f, 1.0f, 0.0f }, .uv = Vec2{ 0, 1 } },  // 左下
			Vertex{ .position = Vec3{ 1.0f, 1.0f, 0.0f }, .uv = Vec2{ 1, 1 } },  // 右下
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

		surfaceQuad = { std::move(surface) };
	});
}

void UISystem::Update()
{
	UpdateText();
	UpdateRenderer();
	UpdateRaycast();
}

void UISystem::UpdateRenderer()
{
	auto viewImage = GetView<UITransform, UIImage>();

	viewImage.ForEachWithIndex([this]
	(const size_t _index, IObject& _object, UITransform& _transform, UIImage& _spriteRenderer)
	{
		// NOTE: ラムダ式のキャプチャの関係上ここにポインターを持つ
		auto* pTransform = &_transform;
		auto* pSpriteRenderer = &_spriteRenderer;

		Get<UIManager>().AddRenderer([this, pTransform, pSpriteRenderer]
		{
			// UI用にマテリアル送信
			Dx11Manager& dx11 = Get<Dx11Manager>();
			dx11.ChangeDepthState(DepthStateType_None);
			dx11.SetBlendState(BlendStateType_Alpha);  // 透過描画もする

			dx11.SetShader(uiShader);  // UI描画用シェーダをセット
			dx11.SetMaterial(pTransform->GetMaterial());  // UITransformのマテリアルをセット

			SpriteRendererMaterialProperty* pSpriteRendererMaterialProperty = static_cast<SpriteRendererMaterialProperty*>(material.Get().pProperty.get());

			// TODO: _spriteRenderer.のテクスチャを参照する
			(*pSpriteRendererMaterialProperty)->hasTexture = { pSpriteRenderer->texture, FALSE, FALSE, FALSE };
			(*pSpriteRendererMaterialProperty)->baseColor = pSpriteRenderer->baseColor;

			dx11.SetMaterial(material);
			if (pSpriteRenderer->texture)
			{
				dx11.SetTexture(pSpriteRenderer->texture);  // テクスチャがあるなら送信する
			}
			dx11.SetSurface(surfaceQuad);
			dx11.Draw();
			dx11.ChangeDepthState(DepthStateType_Standard);
			dx11.SetBlendState(BlendStateType_None);
		}, pTransform->layerOrder.WithUIType(UILayerOrder::UIType_Figure));
	});
}

void UISystem::UpdateText()
{
	auto viewText = GetView<UITransform, UIText>();
	viewText.ForEach([this](IObject& _object, UITransform& _transform, UIText& _text)
	{
		// NOTE: ラムダ式のキャプチャの関係上ここにポインターを持つ
		auto* pTransform = &_transform;
		auto* pText = &_text;

		Get<UIManager>().AddRenderer([this, pTransform, pText]
		{
			D2DManager& d2d = Get<D2DManager>();
			d2d.DrawString(pText->content, pTransform->position, pTransform->scale);
		}, pTransform->layerOrder.WithUIType(UILayerOrder::UIType_Text), true);
	});
}

void UISystem::UpdateRaycast()
{
	RawMouseInput& mouse = Get<RawMouseInput>();
	const Vec2 CURSOR_POSITION = Get<CursorSystem>().GetPosition();

	using UISortPair = std::pair<UILayerOrder, size_t>;
	std::vector<UISortPair> onCursorUIs{};

	auto viewRaycast = GetView<UITransform, UIRaycastTarget>();
	viewRaycast.ForEachWithIndex([this, &mouse, CURSOR_POSITION, &onCursorUIs](const size_t _index, IObject& _object, UITransform& _transform, UIRaycastTarget& _target)
	{
		// 1フレーム限りのフラグをリセット
		_target.isPressedThisFrame = false;
		_target.isReleasedThisFrame = false;

		// 範囲チェック
		bool isHover = true;
		if (CURSOR_POSITION.x < _transform.position.x || CURSOR_POSITION.y < _transform.position.y ||
			CURSOR_POSITION.x >(_transform.position.x + _transform.scale.x) ||
			CURSOR_POSITION.y >(_transform.position.y + _transform.scale.y))
		{
			isHover = false;
		}

		// 範囲外であっても、現在Pressed状態ならボタンが離されたかをチェックする必要がある
		if (!isHover)
		{
			if (_target.currentState == UIRaycastTargetState_Pressed)
			{
				if (mouse.GetMouseUp(MouseCode_Left))
				{
					_target.isReleasedThisFrame = true;
					_target.currentState = UIRaycastTargetState_Normal;
				}
				else if (!mouse.GetMouse(MouseCode_Left))
				{
					// マウスが押されていないならノーマルに戻す
					_target.currentState = UIRaycastTargetState_Normal;
				}
			}
			else if (_target.currentState > UIRaycastTargetState_Normal)
			{
				_target.currentState = UIRaycastTargetState_Normal;
			}
			return; // 範囲外のUIはソート対象に入れない
		}

		onCursorUIs.emplace_back(_transform.layerOrder, _index);
	});

	// 重なり具合をソートする
	std::sort(onCursorUIs.begin(), onCursorUIs.end(), [](const UISortPair& _left, const UISortPair& _right)
	{
		return _left.first < _right.first;
	});

	// マウス操作をUIが奪ったかのフラグをリセット
	isWantCaptureMouse = false;

	if (onCursorUIs.size() == 0)
	{
		return;  // 何も触れていないなら無視
	}

	// 一番手前にあるUIを取得
	auto[layerOrder, index] = onCursorUIs.back();
	viewRaycast.AtFast(index, [this, &mouse](IObject& _object, UITransform& _transform, UIRaycastTarget& _target)
	{
		// UI上で押された瞬間ならフラグを立てる
		_target.isPressedThisFrame = mouse.GetMouseDown(MouseCode_Left);
		// UI上で離された瞬間ならフラグを立てる
		_target.isReleasedThisFrame = mouse.GetMouseUp(MouseCode_Left);

		// 状態も押されているかで変化させる
		_target.currentState = mouse.GetMouse(MouseCode_Left)
			? UIRaycastTargetState_Pressed
			: UIRaycastTargetState_Hovered;

		isWantCaptureMouse = true;  // UI上を操作しているため奪っている
	});
}
