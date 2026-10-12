
#include "RawMouseInput.h"
#include "../ManagerRegistry.h"
#include "WinWindowManager.h"

LRESULT CursorRawInputHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INPUT)
	{
		HRAWINPUT hRawInput{ reinterpret_cast<HRAWINPUT>(lParam) };
		UINT dwSize{ 0 };

		std::vector<char> buffer{};

		GetRawInputData(hRawInput, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

		if (dwSize == 0)
		{
			return FALSE;  // サイズがないなら確認できない
		}

		buffer.resize(dwSize);
		GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT& rawInput{ *reinterpret_cast<RAWINPUT*>(buffer.data()) };

		if (rawInput.header.dwType == RIM_TYPEMOUSE)
		{
			cha::RawMouseInput& input = cha::ManagerRegistry::Ref<cha::RawMouseInput>();

			input.UpdateButtons(rawInput.data.mouse.usButtonFlags);

			// 移動量の生データを蓄積させる
			input.AddDelta(
				rawInput.data.mouse.lLastX,
				rawInput.data.mouse.lLastY);


			// 垂直ホイール処理
			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				// usButtonDataはSHORTにキャストして符号も扱う
				SHORT wheelDelta = static_cast<SHORT>(rawInput.data.mouse.usButtonData);

				input.AddWheelDelta(wheelDelta);
			}

			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}


	return FALSE;
}

cha::RawMouseInput::RawMouseInput(WinWindowManager& _winWindow) :
	winWindow_{ _winWindow }
{
}

bool cha::RawMouseInput::Initialize()
{
	RAWINPUTDEVICE DEVICE
	{
		.usUsagePage = 0x01,  // 普通のデスクトップ
		.usUsage = 0x02,      // マウス入力を取りたい
		.dwFlags = RIDEV_INPUTSINK,  // メッセージを強制的に受け取ることができる
		.hwndTarget = winWindow_.GetWndHandle(),
	};

	// 生データ取得用の登録に成功 true / false
	bool succeed =  RegisterRawInputDevices(&DEVICE, 1, sizeof(DEVICE));

	return succeed;
}

bool cha::RawMouseInput::Update()
{
	// マウス移動量更新
	prevRawDelta_ = rawDelta_;
	rawDelta_ = Vec2Int::Zero();

	// ホイール回転量更新
	prevScrollWheel_ = rawScrollWheel_;
	rawScrollWheel_ = 0;

	// 前フレームの内容をコピー
	buttonStatePrevious = buttonStateCurrent;
	// 現フレームの内容を更新
	buttonStateCurrent = buttonStateRaw;

	return true;
}

void cha::RawMouseInput::AddDelta(const LONG _lLastX, const LONG _lLastY)
{
	rawDelta_.x += _lLastX;
	rawDelta_.y += _lLastY;
}

void cha::RawMouseInput::AddWheelDelta(const SHORT _scroll)
{
	rawScrollWheel_ += _scroll;
}

void cha::RawMouseInput::UpdateButtons(const uint16_t _buttonFlags)
{
	//for (uint16_t buttonCode = 0; buttonCode < MouseCode_Max; buttonCode++)
	//{
	//	// down したとき
	//	buttonStateRaw[buttonCode] = _buttonFlags & (1 << (buttonCode * 2));
	//	
	//	// up したとき
	//	buttonStateRaw[buttonCode] = _buttonFlags & (1 << (buttonCode * 2 + 1));
	//}

	// 左ボタン (MouseCode_Left)
	if (_buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)  buttonStateRaw[MouseCode_Left] = true;
	if (_buttonFlags & RI_MOUSE_LEFT_BUTTON_UP)    buttonStateRaw[MouseCode_Left] = false;

	// 右ボタン (MouseCode_Right)
	if (_buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) buttonStateRaw[MouseCode_Right] = true;
	if (_buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP)   buttonStateRaw[MouseCode_Right] = false;

	// 中央ボタン (MouseCode_Middle)
	if (_buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) buttonStateRaw[MouseCode_Middle] = true;
	if (_buttonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)   buttonStateRaw[MouseCode_Middle] = false;
}

bool cha::RawMouseInput::GetMouse(const MouseCode _mouseCode) const
{
	// 現在のボタン状況を返す
	return buttonStateRaw[_mouseCode];
}

bool cha::RawMouseInput::GetMouseDown(const MouseCode _mouseCode) const
{
	// 前回が押されていないが今回押されているなら押された瞬間
	return !buttonStatePrevious[_mouseCode] && buttonStateCurrent[_mouseCode];
}

bool cha::RawMouseInput::GetMouseUp(const MouseCode _mouseCode) const
{
	// 前回が押されているが今回押されていないなら離された瞬間
	return buttonStatePrevious[_mouseCode] && !buttonStateCurrent[_mouseCode];
}
