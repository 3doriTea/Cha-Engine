
#include "PlayerCamera.h"
#include "PlayerCamera/IPlayerCamera.h"
#include "../System/UISystem.h"
#include "../System/RaycastSystem.h"

void PlayerCamera::Start()
{
	Get<CameraSystem>().Data().transform.position = startPosition;

	playCameraCommon.position = startPosition;

	ChangeType(currentType);
}

void PlayerCamera::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
	{
		// ImGui操作中なら無視
		return;
	}

	if (Get<UISystem>().WantCaptureMouse())
	{
		return;  // UI操作中なら無視
	}

	RawMouseInput& rawInput = Get<RawMouseInput>();
	InputSystem& input = Get<InputSystem>();
	CursorSystem& cursor = Get<CursorSystem>();

	if (rawInput.GetMouseDown(MouseCode_Left))
	{
		isHideCursor = true;
		cursor.SetVisible(false);  // 非表示に
		cursor.ChangeMode(CursorMode_CenterLock);
	}

	if (input.GetKeyDown(KeyCode::C))
	{
		// TODO: switch case にする
		if (currentType == PlayCameraType_TPS)
		{
			ChangeType(PlayCameraType_Spectator);
		}
		else if (currentType == PlayCameraType_Spectator)
		{
			ChangeType(PlayCameraType_TPS);
		}
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

	Transform& camTransform = Get<CameraSystem>().Data().transform;
	Vec3 camPos = camTransform.position;
	RaycastHitInfo hit{};
	if (Get<RaycastSystem>()
		.Raycast(camPos, camTransform.Forward(), 300, &hit))
	{
		if (hit.pObject)
		{
			GameObject hitObj = hit.pObject->ToGameObject();
			if (Name* pName = TryGet<Name>(hitObj); pName)
			{
				if (pName->content == "TestAlphaCube")
				{
					if (input.GetKey(KeyCode::M))
					{
						Get<Transform>(hitObj).position = camTransform.position + camTransform.Forward() * 10.0f;
					}
				}
			}
		}
	}
}

void PlayerCamera::ChangeType(const PlayerCameraType _type)
{
	// 現在の共通設定を取得し無効化
	playCameraCommon = Get<IPlayerCamera>(playerCamera[currentType]).Deactivate();

	// 現在の種類を追加
	currentType = _type;

	// 共通設定を引継ぎ有効化
	Get<IPlayerCamera>(playerCamera[currentType]).Activate(playCameraCommon);
}