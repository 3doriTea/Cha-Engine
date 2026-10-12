#pragma once

/// @brief マウスボタンの識別番号
using MouseCode = uint16_t;

/// @brief マウスボタンの識別番号
enum MouseCode_ : uint16_t
{
	MouseCode_Left = 0,
	MouseCode_Right,
	MouseCode_Middle,
	MouseCode_Max,
};
