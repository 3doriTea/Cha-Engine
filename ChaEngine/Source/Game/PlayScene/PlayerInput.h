#pragma once
#include <Game/Game.h>

/// @brief プレイヤーの入力情報のデータ
struct PlayerInputData
{
	bool jumpOn{};   ///< ジャンプした瞬間
	bool jumping{};  ///< ジャンプ中
	Vec3 move{};     ///< 移動方向 0.0f ~ 1.0f

	bool toolActionMain{};    ///< メインのツールアクション
	bool toolActionMainOn{};  ///< メインのツールアクションが押された瞬間
	bool toolActionSub{};     ///< サブのツールアクション
	bool toolActionSubOn{};   ///< サブのツールアクションが押された瞬間

	int32_t toolScroll{};  ///< ツールのホイールスクロール入力
};

/// @brief プレイヤーの入力情報
struct PlayerInput : ObjectBase<PlayerInputData>
{
	/// @brief 更新処理
	void Update() override;
};
