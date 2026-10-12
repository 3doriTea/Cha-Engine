
#include "DebugCamera.h"

void DebugCamera::Start()
{
	CameraSystemData& cameraSystemData = Get<CameraSystem>().Data();
	cameraSystemData.transform.position = { 0.0f, 0.0f, -10.0f };
}

void DebugCamera::Update()
{

	CursorSystem& cursor = Get<CursorSystem>();
	InputSystem& input = Get<InputSystem>();
	RawMouseInput& rawInput = Get<RawMouseInput>();
	CameraSystemData& cameraSystemData = Get<CameraSystem>().Data();

	ImGui::Begin("DebugCamera");

	ImGui::DragFloat3("position", cameraSystemData.transform.position.v);

	ImGui::End();

	//#ifdef _DEBUG
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
	{
		// ImGui操作中なら無視
		return;
	}
	//#endif

	
	if (rawInput.GetMouseDown(MouseCode_Left))
	{
		isHideCursor = true;
		cursor.SetVisible(false);  // 非表示に
		cursor.ChangeMode(CursorMode_CenterLock);
	}

	if (!isHideCursor)
	{
		return;  // カーソルが非表示なら操作しない
	}

	if (input.GetKeyDown(KeyCode::Escape))
	{
		isHideCursor = false;
		cursor.SetVisible(true);  // 表示する
		cursor.ChangeMode(CursorMode_Free);
		return;
	}

	if (cursor.IsVisible())
	{
		return;  // カーソルが表示されているなら操作しない
	}

	float rotateSpeed{ rotateSpeedPerSec * Get<DeltaTime>() };

	angles.x += cursor.Data().moveY * rotateSpeed;
	angles.y += cursor.Data().moveX * rotateSpeed;

	// 角度に制限をつける
	angles.x = std::clamp(
		angles.x,
		-DirectX::XM_PIDIV2 + 0.01f,
		DirectX::XM_PIDIV2 - 0.01f);

	cameraSystemData.transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(
		angles.x,
		angles.y,
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

	if (!input.GetKey(KeyCode::LeftControl))
		move *= 10000.0f;

	move *= moveSpeedPerSec * Get<DeltaTime>();

	cameraSystemData.transform.position += move;
}
