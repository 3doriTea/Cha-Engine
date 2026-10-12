#pragma once
#include <Game/Game.h>
#include "IPlayerCamera.h"

/// @brief カメラ: TPSモード のデータ
struct CameraTPSData
{
	/// @brief 初期化処理
	/// @param _target ターゲットとなるオブジェクト
	/// @param _terrain 地形情報
	CameraTPSData(const GameObject _target, const GameObject _terrain) :
		target{ _target },
		terrain{ _terrain }
	{
	}

	GameObject target{};   ///< カメラが追従するオブジェクト
	GameObject terrain{};  ///< 地形オブジェクト

	float rotateSpeedPerSec{ 0.1f };  ///< 1秒間に回転する角度
	PlayerCameraCommon common{};
	bool isActive{ false };

	float terrainHeightOffsetY{ 0.01f };  ///< 地面にカメラが埋まらないように底上げ

	Vec3 localOffset{ 0.0f, 2.0f, -10.0f };  ///< プレイヤーの位置からカメラを引いたベクトル
};

/// @brief カメラ: 三人称後方視点
struct CameraTPS : ObjectBase<CameraTPSData>, IPlayerCamera
{
	using ObjectBase<CameraTPSData>::ObjectBase;

	/// @brief 開始処理
	void Start();
	/// @brief 更新処理
	void Update();

	/// @brief 有効化する
	/// @param _common 引継ぎ共通項目
	void Activate(const PlayerCameraCommon& _common) override;
	/// @brief 無効化する
	/// @return 引継ぎ共通項目
	PlayerCameraCommon Deactivate() override;

private:
	/// @brief 地面にカメラが埋まっていれば戻す処理
	/// @param _toPlayerPosition 想定の座標
	/// @return 戻す処理が発生した true / false
	bool ResolveTerrainCollision(Vec3& _toPlayerPosition);
};