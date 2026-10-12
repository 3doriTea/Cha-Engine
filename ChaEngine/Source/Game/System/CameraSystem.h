#pragma once
#include <Base/Base.h>
#include "../Component/Transform.h"

/// @brief カメラの投影モード
enum struct CameraSystemProjectionMode
{
	Orthographic,  // 並行投影
	Perspective,   // 透視投影
};

struct CameraSystemData
{
	Component<Transform> transform{};
	Mat4x4 viewMatrix{ Mat4x4::Identity() };        // ビュー行列
	Mat4x4 projectionMatrix{ Mat4x4::Identity() };  // プロジェクト行列
	float nearClipDistance{ 0.1f };
	float farClipDistance{ 1000.0f };

	float fieldOfViewRad{ DirectX::XM_PIDIV4 };   // FOVのラジアン
	Vec2 orthographicSize{ 500, 500 };  // 並行投影の場合のサイズ

	// 現在の投影モード
	CameraSystemProjectionMode projectionMode{ CameraSystemProjectionMode::Perspective };

	Resource<ResourceType::Material> cameraMaterial{};
};

struct CameraSystem : SystemObjectBase<CameraSystem, CameraSystemData>
{
	void Start() override;
	void Update() override;

	/// @brief 投影モードを設定する
	/// @param _mode 設定する投影モード
	inline void SetProjectionMode(CameraSystemProjectionMode _mode) { projectionMode = _mode; }
};
