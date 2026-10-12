#pragma once
#include <Game/Game.h>
#include "PlayerCamera/PlayerCameraCommon.h"

/// @brief プレイカメラモード
using PlayerCameraType = uint32_t;

/// @brief プレイカメラモード
enum PlayerCameraType_ : PlayerCameraType
{
	PlayCameraType_Spectator,  ///< 観戦者モード
	PlayCameraType_TPS,        ///< TPSモード
	PlayCameraType_Max,        ///< プレイカメラモードの最大数
};

/// @brief 抽象的プレイヤーカメラのデータ
struct PlayerCameraData
{
	/// @brief 初期化処理
	/// @param _objPlayer プレイヤーオブジェクト
	/// @param _playerCameras プレイヤーカメラたち
	inline PlayerCameraData(
		const GameObject _objPlayer,
		std::array<GameObject, PlayCameraType_Max>&& _playerCameras) :
		playerCamera{ std::move(_playerCameras) },
		objPlayer{ _objPlayer }
	{
	}

	GameObject objPlayer{};  ///< プレイヤーのオブジェクト

	PlayerCameraType currentType{ PlayCameraType_Spectator };  ///< 現在のプレイヤーカメラの種類
	std::array<GameObject, PlayCameraType_Max> playerCamera;  ///< プレイヤーカメラたち

	Vec3 startPosition{ 0, 0, -10 };  ///< カメラの開始地点

	PlayerCameraCommon playCameraCommon{};  ///< カメラ共有の設定

	bool isHideCursor{ false };  ///< カーソルを隠すか
};

/// @brief 抽象的プレイヤーカメラ
struct PlayerCamera : ObjectBase<PlayerCameraData>
{
	using ObjectBase<PlayerCameraData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief プレイカメラの現在のモードを取得する
	/// @return プレイカメラのモード
	inline PlayerCameraType GetCurrentType() const { return currentType; }

	/// @brief カメラモードを変更する
	/// @param _type 変更先カメラモード
	void ChangeType(const PlayerCameraType _type);
};