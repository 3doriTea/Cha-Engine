#pragma once
#include <string_view>
#include <format>
#include <Windows.h>
#include "VCUWide.h"

#define VCU_ASSERT(expr) do                                                \
{                                                                          \
	if (!expr)                                                             \
	{                                                                      \
		std::wstring text = std::format(                                   \
			VCU_WIDE("式:{}\r\nファイル:{}\r\n行数:{}\r\n最終エラーコード:{}"),\
			VCU_WIDE(#expr),                                               \
			VCU_WIDE(__FILE__),                                            \
			(unsigned int)__LINE__,                                        \
			GetLastError());                                               \
		std::wstring caption{ L"Assertion Failed!" };                      \
		UINT type{ MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_DEFBUTTON2 };    \
		int ret = MessageBox(nullptr, text.c_str(), caption.c_str(), type);\
		switch (ret)                                                       \
		{                                                                  \
		case IDRETRY:                                                      \
			__debugbreak();                                                \
			break;                                                         \
		case IDABORT:                                                      \
		case IDIGNORE:                                                     \
		default:                                                           \
			PostQuitMessage(-1);                                           \
			break;                                                         \
		}                                                                  \
	}                                                                      \
}                                                                          \
while (false)
