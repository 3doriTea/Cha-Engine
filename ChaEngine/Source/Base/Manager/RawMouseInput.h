#pragma once
#include "IManager.h"
#include "../BasePch.h"
#include "RawMouseInput/MouseCode.h"


/// @brief カーソルの生データ受信用ハンドラ
/// @param hWnd 
/// @param msg 
/// @param wParam 
/// @param lParam 
/// @return 
LRESULT CursorRawInputHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace cha
{
	class WinWindowManager;

	/// @brief 生のマウスデータを取得する
	class RawMouseInput : public IManager
	{
	public:
		RawMouseInput(WinWindowManager& _winWindow);

		/// @brief 初期化処理
		/// @return 初期化処理に成功 true / false
		bool Initialize();

		/// @brief 1フレームの更新処理
		/// @return 更新成功 true / false
		bool Update();

		/// @brief 1フレーム中の移動量(生データ)を追加する
		/// @param _lLastX 生データ x
		/// @param _lLastY 生データ y
		void AddDelta(const LONG _lLastX, const LONG _lLastY);

		/// @brief 1フレーム中のホイール回転量(生データ)を追加する
		/// @param _scroll 回転量
		void AddWheelDelta(const SHORT _scroll);

		/// @brief ボタンのフラグを更新する
		/// @param _buttonFlags ボタンのフラグ
		void UpdateButtons(const uint16_t _buttonFlags);

		/// @brief 1フレーム間の移動量(生データ)を取得する
		/// @return 移動量 (生データ)
		inline Vec2Int GetMoveDelta() const { return prevRawDelta_; }

		/// @brief マウスの指定ボタンが押されているか取得
		/// @param _mouseCode マウスの指定ボタン
		/// @return 押されている true / false
		bool GetMouse(const MouseCode _mouseCode) const;
		/// @brief マウスの指定ボタンが押された瞬間か取得
		/// @param _mouseCode マウスの指定ボタン
		/// @return 離された瞬間のフレーム true / false
		bool GetMouseDown(const MouseCode _mouseCode) const;
		/// @brief マウスの指定ボタンが離された瞬間か取得
		/// @param _mouseCode マウスの指定ボタン
		/// @return 離された瞬間のフレーム true / false
		bool GetMouseUp(const MouseCode _mouseCode) const;

		/// @brief スクロールホイールの回転量を取得
		/// @return 回転量
		inline int32_t GetScrollWheel() const { return prevScrollWheel_; }

	private:
		std::array<bool, MouseCode_Max> buttonStateRaw{};
		std::array<bool, MouseCode_Max> buttonStateCurrent{};
		std::array<bool, MouseCode_Max> buttonStatePrevious{};

		Vec2Int rawDelta_{};      // 移動量
		Vec2Int prevRawDelta_{};  // 前回の移動量

		int32_t rawScrollWheel_{};  // ホイールスクロール回転量
		int32_t prevScrollWheel_{};  // 前回の回転量

		WinWindowManager& winWindow_;  // ウィンドウマネージャ
	};
}
