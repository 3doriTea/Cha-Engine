#pragma once
#include "../../BasePch.h"

namespace cha
{
	/// @brief 深度バッファへの書き込み方法
	using DepthStateType = uint32_t;

	/// @brief 深度バッファへの書き込み方法
	enum DepthStateType_ : DepthStateType
	{
		DepthStateType_None,        ///< 深度ステートなし
		DepthStateType_Standard,    ///< 普通
		DepthStateType_Background,  ///< 最背面
		DepthStateType_Max,         ///< 最大数
	};
}
