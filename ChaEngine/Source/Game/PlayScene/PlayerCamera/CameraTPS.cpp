
#include "CameraTPS.h"
#include <Game/PlayScene/IPlayer.h>
#include <Game/PlayScene/Terrain.h>

void CameraTPS::Start()
{

}

void CameraTPS::Update()
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

#pragma region カメラの回転処理
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
#pragma endregion

#pragma region カメラの追従処理
	Transform& playerTransform = Get<Transform>(target);
	Vec3 playerPosition = playerTransform.position;

	// カメラのxyオイラー角の回転行列を求める
	Mat4x4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(common.angles.x, common.angles.y, 0.0f);

	// 回転行列をかけプレイヤーへの座標を求める
	Vec3 localPosition = localOffset * rotationMatrix;
	Vec3 toPlayerPosition = playerPosition + localPosition;

	// 地形にあたっていたら上げる
	ResolveTerrainCollision(toPlayerPosition);

	common.position = toPlayerPosition;

	cameraSystemData.transform.position = common.position;
#pragma endregion

	Get<IPlayer>(target).SetTargetRotation(
		DirectX::XMQuaternionRotationRollPitchYaw(0.0f, common.angles.y, 0.0f));
}

void CameraTPS::Activate(const PlayerCameraCommon& _common)
{
	// プレイヤーの操作を有効化する
	Get<IPlayer>(target).SetActiveControl(true);

	common = _common;
	isActive = true;
}

PlayerCameraCommon CameraTPS::Deactivate()
{
	// プレイヤーの操作を無効化する
	Get<IPlayer>(target).SetActiveControl(false);

	isActive = false;
	return common;
}

bool CameraTPS::ResolveTerrainCollision(Vec3& _toPlayerPosition)
{
	float terrainHeight = Get<Terrain>(terrain).GetHeight(_toPlayerPosition.x, _toPlayerPosition.z);
	float collisionHeight = terrainHeight + terrainHeightOffsetY;
	if (_toPlayerPosition.y < collisionHeight)
	{
		_toPlayerPosition.y = collisionHeight;
		return true;
	}
	return false;
}