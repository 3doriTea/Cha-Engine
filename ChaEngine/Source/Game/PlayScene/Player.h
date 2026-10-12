#pragma once
#include <Game/Game.h>
#include "PlayerConfig.h"
#include "PlayerState.h"
#include "IPlayer.h"

/// @brief プレイヤーのデータ
struct PlayerData
{
	inline PlayerData(
		const GameObject _objPlayerInput,
		const GameObject _objTerrain,
		const GameObject _objMessageArea) :
		objPlayerInput{ _objPlayerInput },
		objTerrain{ _objTerrain },
		objMessageArea{ _objMessageArea }
	{
		VCU_LOG("instantiated PlayerData");
	}

	inline ~PlayerData()
	{
		VCU_LOG("destruct PlayerData");
	}

	Component<Name> name{ "Player" };      ///< 名前コンポーネント
	Component<Transform> transform{};      ///< 座標系
	Component<DynamicOBB> obb{};           ///< OBBコライダー
	Component<OBBRenderer> obbRenderer{};  ///< obb描画コンポーネント

	//Component<ImTransform> imTransform{};

	int stakeIdCounter{};

	GameObject prevInstance{};  ///< 1つ前のインスタンス
	
	GameObject objHpGauge{};  ///< HPゲージUI

	Resource<Shader> shader{ "Shader/Unlit.hlsl" };  ///< シェーダ

	/*Resource<Model> model{ "Model/Crafter/Crafter3.fbx" };*/

	//Resource<Model> model{ "Model/kippo/kippoascii.fbx" };
	Resource<Model> model{ "Model/Actor/Actor.fbx" };



	GameObject objPlayerInput{};  ///< プレイヤーの入力情報オブジェクト
	GameObject objTerrain{};
	GameObject objMessageArea{};  ///< 字幕ボックス

	PlayerState state{};  ///< プレイヤーの状態設定

	PlayerConfig config{};  ///< プレイヤーの設定
	//NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerData, config);

	float jumpSpeedPerSec{};  // ジャンプスピード(秒)

	using AnimStatus = uint32_t;
	enum AnimStatus_ : AnimStatus
	{
		AnimStatus_Idle,
		AnimStatus_Run,
		AnimStatus_Max,
	};

	AnimStatus animStatus{ AnimStatus_Idle };  ///< アニメーションステータス
};

/// @brief プレイヤー
struct Player : ObjectBase<PlayerData>, /*JsonLoader,*/ IPlayer
{
	/// @brief 初期化処理
	/// @param _objPlayerInput プレイヤーの入力情報オブジェクト
	Player(const GameObject _objPlayerInput, const GameObject _objTerrain, const GameObject _objMessageArea);

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 操作を有効化するかを設定
	/// @param _isActive 有効化する true / false
	inline void SetActiveControl(const bool _isActive) override { state.isActiveControl = _isActive; }

	/// @brief 自動回転目標を設定する
	/// @param _targetRotation 目標の回転
	inline void SetTargetRotation(const Quaternion& _targetRotation) override { state.targetRotation = _targetRotation; }

private:
	/// @brief 地形により移動ができないかを取得
	/// @return 移動できない true / false
	bool IsBlockedTerrain();

	/// @brief 座標を地形の高さに合わせる
	/// @return 地形にスナップしたか true / false
	bool SnapTerrain();

	/// @brief JSON読み込んだ時の処理
	/// @param _json JSON JSON
	//inline void OnLoadJson(json& _json) override { _json.get_to(Data()); }

	/// @brief プレイヤーのy軸における地形の高さを取得する
	/// @return 地形の高さ
	const float GetTerrainHeight();
};
