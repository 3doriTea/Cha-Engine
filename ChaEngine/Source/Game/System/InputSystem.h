#pragma once
// 古いDIRECTINPUT8でのマウス入力を行うか
#define USE_OLD_DIRECTINPUT8_MOUSE FALSE

#include <Base/Base.h>
#include "InputSystem/KeyCode.h"

#if USE_OLD_DIRECTINPUT8_MOUSE
#include "InputSystem/MouseCode.h"
#endif

struct InputSystemData
{
	using KeyBoardStateArray = std::array<BYTE, KEY_BOARD_STATE_MAX>;

	// Direct Input 使うインタフェース
	ComPtr<IDirectInput8> pDirectInput{};

	struct
	{
		// デバイスインタフェースインタフェース
		ComPtr<IDirectInputDevice8> pDevice{};

		// 現フレームでのキーボード状態
		KeyBoardStateArray stateCurr{};
		// 前フレームでのキーボード状態
		KeyBoardStateArray statePrev{};
	} keyBoard{};  // キーボードデバイスの情報

#if USE_OLD_DIRECTINPUT8_MOUSE
	struct
	{
		// デバイスインタフェースインタフェース
		ComPtr<IDirectInputDevice8> pDevice{};

		// 現フレームでのマウス座標
		Vec2Int positionCurr{};
		// 前フレームでのマウス座標
		Vec2Int positionPrev{};
		// 現フレームでのマウス入力
		DIMOUSESTATE stateCurr{};
		// 前フレームでのマウス入力
		DIMOUSESTATE statePrev{};
		// 現フレームでのマウス座標
		Vec2Int mousePositionCurr{};
		// 前フレームでのマウス座標
		Vec2Int mousePositionPrev{};
	} mouse{};  // マウスデバイスの情報
#endif

	struct
	{
		// 現フレームでのゲームパッド状態
		XINPUT_STATE padStateCurr{};
		// 前フレームでのゲームパッド状態
		XINPUT_STATE padStatePrev{};
	} controller{};  // ゲームコントローラ ゲームパッド
};

struct InputSystem : SystemObjectBase<InputSystem, InputSystemData>
{
	void Start() override;
	void Update() override;

	/// @brief キーが押された瞬間かを取得
	/// @param _keyCode 指定キー
	/// @return 押された瞬間 true / false
	bool GetKeyDown(const KeyCode _keyCode) const;
	/// @brief キーが押されているかを取得
	/// @param _keyCode 指定キー
	/// @return キーが押されている true / false
	bool GetKey(const KeyCode _keyCode) const;
	/// @brief キーが離された瞬間かを取得
	/// @param _keyCode 指定キー
	/// @return キーが離された瞬間 true / false
	bool GetKeyUp(const KeyCode _keyCode) const;

#if USE_OLD_DIRECTINPUT8_MOUSE
	/// @brief マウスが押された瞬間かを取得
	/// @param _mouseCode 指定マウス
	/// @return 押された瞬間 true / false
	bool GetMouseDown(const MouseCode _mouseCode) const;
	/// @brief マウスが押されているかを取得
	/// @param _mouseCode 指定マウス
	/// @return マウスが押されている true / false
	bool GetMouse(const MouseCode _mouseCode) const;
	/// @brief マウスが離された瞬間かを取得
	/// @param _mouseCode 指定マウス
	/// @return マウスが離された瞬間 true / false
	bool GetMouseUp(const MouseCode _mouseCode) const;
#endif

private:
	using SystemObjectBase<InputSystem, InputSystemData>::Data;
};
