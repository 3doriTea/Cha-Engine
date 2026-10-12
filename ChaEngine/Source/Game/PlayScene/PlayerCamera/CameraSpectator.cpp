
#include "CameraSpectator.h"


void CameraSpectator::Start()
{
}

void CameraSpectator::Update()
{
	if (!isActive)
	{
		return;  // 非アクティブなら無視
	}

	//#ifdef _DEBUG
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
	{
		// ImGui操作中なら無視する
		return;
	}
	//#endif

	CursorSystem& cursor = Get<CursorSystem>();
	InputSystem& input = Get<InputSystem>();
	CameraSystemData& cameraSystemData = Get<CameraSystem>().Data();

	if (cursor.IsVisible())
	{
		return;  // カーソルが表示されているなら操作しない
	}

	float rotateSpeed{ rotateSpeedPerSec * Get<DeltaTime>() };

	common.angles.x += cursor.Data().moveY * rotateSpeed;
	common.angles.y += cursor.Data().moveX * rotateSpeed;

	// 角度に制限をつける
	common.angles.x = std::clamp(
		common.angles.x,
		-DirectX::XM_PIDIV2 + 0.01f,
		DirectX::XM_PIDIV2 - 0.01f);

	cameraSystemData.transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(
		common.angles.x,
		common.angles.y,
		0.0f);

	Vec3 move{};

	if (input.GetKey(KeyCode::W))
		move += cameraSystemData.transform.Forward();
	if (input.GetKey(KeyCode::A))
		move += cameraSystemData.transform.Left();
	if (input.GetKey(KeyCode::S))
		move += cameraSystemData.transform.Back();
	if (input.GetKey(KeyCode::D))
		move += cameraSystemData.transform.Right();
	if (input.GetKey(KeyCode::Q))
		move += cameraSystemData.transform.Down();
	if (input.GetKey(KeyCode::E))
		move += cameraSystemData.transform.Up();


	move *= moveSpeedPerSec * Get<DeltaTime>();

	cameraSystemData.transform.position += move;
}

void CameraSpectator::Activate(const PlayerCameraCommon& _common)
{
	common = _common;
	isActive = true;
}

PlayerCameraCommon CameraSpectator::Deactivate()
{
	isActive = false;
	return common;
}