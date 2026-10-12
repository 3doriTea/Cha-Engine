#include "CursorSystem.h"
#include <Base/Manager/WinWindowManager.h>

void CursorSystem::Start()
{
	// ウィンドウハンドルを取得しておく
	hWnd = Get<WinWindowManager>().GetWndHandle();

	// NOTE: 前のマウスカーソル位置を捨てるためにあえて2回呼ぶ
	UpdatePosition();
	UpdatePosition();
}

void CursorSystem::Update()
{
	if (GetForegroundWindow() != hWnd)
	{
		ClipCursor(nullptr);
		return;  // 最前面ではないなら無視
	}

	// ウィンドウ範囲を取得
	RECT rect{};
	GetWindowRect(hWnd, &rect);

	// マウス座標を更新
	UpdatePosition();

	switch (mode)
	{
	case CursorMode_CenterLock:
		UpdateCenterLock(rect);
		break;
	case CursorMode_WindowLock:
		UpdateWindowLock(rect);
		break;
	case CursorMode_Free:
	default:
		UpdateFree(rect);
		break;
	}

	if (deltaX != 0 || deltaY != 0)
	{
		// カーソルの移動を求める
		moveX = static_cast<float>(deltaX) * mouseSensitivity;
		moveY = static_cast<float>(deltaY) * mouseSensitivity;
	}
	else
	{
		moveX = 0.0f;
		moveY = 0.0f;
	}
}

void CursorSystem::SetVisible(bool _visible)
{
	if (_visible)
	{
		// カウンタが0以上になるまでTRUEを呼び続ける
		while (ShowCursor(TRUE) < 0);
	}
	else
	{
		// カウンタが0未満になるまでFALSEを呼び続ける
		while (ShowCursor(FALSE) >= 0);
	}
}

bool CursorSystem::IsVisible() const
{
	// カーソルの状態を取得する
	CURSORINFO ci{};
	ci.cbSize = sizeof(CURSORINFO);

	// カーソル情報を取得
	if (GetCursorInfo(&ci))
	{
		// flagsがCURSOR_SHOWINGであれば表示されている
		return (ci.flags & CURSOR_SHOWING) != 0;
	}
	
	return false;
}

Vec2Int CursorSystem::GetPosition() const
{
	POINT clientPoint{ currentPosition.x, currentPosition.y };

	if (hWnd != nullptr)
	{
		ScreenToClient(hWnd, &clientPoint);
	}

	return { clientPoint.x, clientPoint.y };
}

void CursorSystem::UpdatePosition()
{
	// 現在のカーソル位置を取得
	POINT currentPoint{};
	if (GetCursorPos(&currentPoint) == FALSE)
	{
		VCU_LOG("Warning: カーソル座標取得失敗");
		return;  // カーソル座標取得失敗のため無視
	}

	previousPosition = currentPosition;
	currentPosition.x = currentPoint.x;
	currentPosition.y = currentPoint.y;
}

void CursorSystem::UpdateFree(const RECT& _rect)
{
	// 移動量を計算
	deltaX = static_cast<float>(currentPosition.x - previousPosition.x);
	deltaY = static_cast<float>(currentPosition.y - previousPosition.y);
}

void CursorSystem::UpdateWindowLock(const RECT& _rect)
{
	ClipCursor(nullptr);

	// クライアント座標（0,0基準）をスクリーン座標（画面全体基準）に変換
	POINT topLeft{ _rect.left, _rect.top };
	POINT bottomRight{ _rect.right, _rect.bottom };
	ClientToScreen(hWnd, &topLeft);
	ClientToScreen(hWnd, &bottomRight);

	// 閉じ込め範囲
	RECT clipRect{};
	clipRect.left = topLeft.x;
	clipRect.top = topLeft.y;
	clipRect.right = bottomRight.x;
	clipRect.bottom = bottomRight.y;

	// 移動量を計算
	deltaX = static_cast<float>(currentPosition.x - previousPosition.x);
	deltaY = static_cast<float>(currentPosition.y - previousPosition.y);

	// マウスカーソルを閉じ込める
	ClipCursor(&clipRect);
}

void CursorSystem::UpdateCenterLock(const RECT& _rect)
{
	ClipCursor(nullptr);

	// ウィンドウの中心を取得
	int centerX{ _rect.left + (_rect.right - _rect.left) / 2 };
	int centerY{ _rect.top + (_rect.bottom - _rect.top) / 2 };

	// 移動量を取得
	deltaX = static_cast<float>(currentPosition.x - centerX);
	deltaY = static_cast<float>(currentPosition.y - centerY);

	// 少しでも動いたなら
	if (deltaX != 0 || deltaY != 0)
	{
		// カーソルを中心に持っていく
		SetCursorPos(centerX, centerY);
	}
}
