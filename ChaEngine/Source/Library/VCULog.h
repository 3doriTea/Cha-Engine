#ifdef _DEBUG
#include <windows.h>
#include <format>
#include <string>

#define VCU_LOG(fmt, ...) do { \
	std::string utf8_str = std::format(fmt "\n", ##__VA_ARGS__); \
	\
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0); \
	if (wlen > 0) { \
		std::wstring wstr(wlen, 0); \
		MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wlen); \
		OutputDebugStringW(wstr.c_str()); \
	} \
} while (false)
#else
#define VCU_LOG(fmt, ...)
#endif