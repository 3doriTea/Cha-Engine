
#include "InputSystem.h"
#include <Base/Manager/WinWindowManager.h>

void InputSystem::Start()
{
	HRESULT hResult{};
	HWND hWnd{ Get<WinWindowManager>().GetWndHandle() };

#pragma region DirectInput8デバイス作成
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(pDirectInput.GetAddressOf()),
		nullptr);
	VCU_ASSERT(SUCCEEDED(hResult) && "DirectInput8の作成に失敗");
	if (FAILED(hResult))
	{
		return;
	}
#pragma endregion

#pragma region キーボードデバイス準備
	hResult = pDirectInput->CreateDevice(
		GUID_SysKeyboard,
		keyBoard.pDevice.GetAddressOf(),
		nullptr);
	VCU_ASSERT(SUCCEEDED(hResult) && "KeyBoardDeviceの作成に失敗");
	if (FAILED(hResult))
	{
		return;
	}

	hResult = keyBoard.pDevice->SetDataFormat(&c_dfDIKeyboard);
	VCU_ASSERT(SUCCEEDED(hResult) && "KeyBoardDeviceのフォーマット設定に失敗");
	if (FAILED(hResult))
	{
		return;
	}

	hResult = keyBoard.pDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	VCU_ASSERT(SUCCEEDED(hResult) && "KeyBoardDeviceの干渉レベル設定に失敗");
	if (FAILED(hResult))
	{
		return;
	}
#pragma endregion

#if USE_OLD_DIRECTINPUT8_MOUSE
#pragma region マウスデバイス準備
	hResult = pDirectInput->CreateDevice(
		GUID_SysMouse,
		mouse.pDevice.GetAddressOf(),
		nullptr);
	VCU_ASSERT(SUCCEEDED(hResult) && "MouseDeviceの作成に失敗");
	if (FAILED(hResult))
	{
		return;
	}

	hResult = mouse.pDevice->SetDataFormat(&c_dfDIMouse);
	VCU_ASSERT(SUCCEEDED(hResult) && "MouseDeviceのフォーマット設定に失敗");
	if (FAILED(hResult))
	{
		return;
	}

	hResult = mouse.pDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	VCU_ASSERT(SUCCEEDED(hResult) && "MouseDeviceの干渉レベル設定に失敗");
	if (FAILED(hResult))
	{
		return;
	}
#pragma endregion
#endif
}

void InputSystem::Update()
{
	// 前フレームにコピー
	keyBoard.statePrev = keyBoard.stateCurr;
	
	// キーボード更新
	if (HRESULT hResult{ keyBoard.pDevice->Acquire() };
		SUCCEEDED(hResult))
	{
		keyBoard.pDevice->GetDeviceState(
			static_cast<DWORD>(keyBoard.stateCurr.size()),
			keyBoard.stateCurr.data());
	}
	else
	{
		// MEMO: 高確率で警告出るため無視
		// VCU_LOG("Warning: Keyboard not acquired");
	}

#if USE_OLD_DIRECTINPUT8_MOUSE
	// 前フレームにコピー
	mouse.statePrev = mouse.stateCurr;

	// マウス更新
	if (HRESULT hResult{ mouse.pDevice->Acquire() };
		SUCCEEDED(hResult))
	{
		mouse.pDevice->GetDeviceState(
			sizeof(mouse.stateCurr),
			&mouse.stateCurr);
	}
	else
	{
		// MEMO: 高確率で警告出るため無視
		// VCU_LOG("Warning: Mouse not acquired");
	}
#endif
}

bool InputSystem::GetKeyDown(const KeyCode _keyCode) const
{
	return keyBoard.stateCurr[static_cast<uint32_t>(_keyCode)]
		&& !keyBoard.statePrev[static_cast<uint32_t>(_keyCode)];
}

bool InputSystem::GetKey(const KeyCode _keyCode) const
{
	return keyBoard.stateCurr[static_cast<uint32_t>(_keyCode)];
}

bool InputSystem::GetKeyUp(const KeyCode _keyCode) const
{
	return !keyBoard.stateCurr[static_cast<uint32_t>(_keyCode)]
		&& keyBoard.statePrev[static_cast<uint32_t>(_keyCode)];
}

#if USE_OLD_DIRECTINPUT8_MOUSE
bool InputSystem::GetMouseDown(const MouseCode _mouseCode) const
{
	return (mouse.stateCurr.rgbButtons[static_cast<uint32_t>(_mouseCode)] & 0x80)
		&& !(mouse.statePrev.rgbButtons[static_cast<uint32_t>(_mouseCode)] & 0x80);
}

bool InputSystem::GetMouse(const MouseCode _mouseCode) const
{
	return (mouse.stateCurr.rgbButtons[static_cast<uint32_t>(_mouseCode)] & 0x80);
}

bool InputSystem::GetMouseUp(const MouseCode _mouseCode) const
{
	return !(mouse.stateCurr.rgbButtons[static_cast<uint32_t>(_mouseCode)] & 0x80)
		&& (mouse.statePrev.rgbButtons[static_cast<uint32_t>(_mouseCode)] & 0x80);
}
#endif
