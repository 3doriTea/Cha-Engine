
#include "CameraSystem.h"
#include <Base/Manager/ScreenSize.h>
#include <Base/Manager/Dx11Manager.h>
#include "CameraSystem/CameraMaterialProperty.h"

void CameraSystem::Start()
{
	using namespace DirectX;

	// スクリーンのサイズ
	Vec2 SCREEN_SIZE = Get<ScreenSize>();

	// 投影モードによる分岐
	switch (projectionMode)
	{
	case CameraSystemProjectionMode::Orthographic:  // 並行投影
		projectionMatrix = XMMatrixOrthographicLH(
			orthographicSize.x,
			orthographicSize.y,
			nearClipDistance,
			farClipDistance);
		break;
	case CameraSystemProjectionMode::Perspective:  // 透視投影
		projectionMatrix = XMMatrixPerspectiveFovLH(
			fieldOfViewRad,
			SCREEN_SIZE.x / SCREEN_SIZE.y,
			nearClipDistance,
			farClipDistance);
		break;
	default:  // 知らない投影
		assert(false && "知らない投影方法");
		break;
	}

	cameraMaterial = MaterialResource{};
	cameraMaterial.Get().SetProperty<CameraMaterialProperty>();
	cameraMaterial.Get().slot = 1;
}

void CameraSystem::Update()
{
	using namespace DirectX;

	viewMatrix = XMMatrixLookAtLH(
		transform.position,
		transform.position + transform.Forward(),
		Vec3::Up());

	MaterialResource& material = cameraMaterial.Get();

	CameraMaterialProperty* pCameraMaterialProperty =
		static_cast<CameraMaterialProperty*>(material.pProperty.get());

	(*pCameraMaterialProperty)->viewProjectionMatrix = viewMatrix * projectionMatrix;
	(*pCameraMaterialProperty)->position = transform.position;

	Get<Dx11Manager>().SetMaterial(cameraMaterial);
}
