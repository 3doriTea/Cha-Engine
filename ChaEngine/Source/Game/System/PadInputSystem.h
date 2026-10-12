#pragma once
#include <Game/Game.h>

/// @brief ゲームパッド入力システムのデータ
struct PadInputSystemData
{
	/// @brief ゲームパッド入力状態
	struct PadState
	{
		uint32_t buttons;  ///< 各ボタンの on/off ビット演算必須

		float leftStickX;  ///< 左スティックのX軸
		float leftStickY;  ///< 左スティックのY軸

		float rightStickX;  ///< 右スティックのX軸
		float rightStickY;  ///< 右スティックのY軸

		float leftTrigger;   ///< 左トリガー
		float rightTrigger;  ///< 右トリガー
	};

	PadState current{};   ///< 今の入力状態
	PadState previous{};  ///< 前の入力状態
};

/// @brief ゲームパッド入力システム
struct PadInputSystem : SystemObjectBase<PadInputSystem, PadInputSystemData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;


};