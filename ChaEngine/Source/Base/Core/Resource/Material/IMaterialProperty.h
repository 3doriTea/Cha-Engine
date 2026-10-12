#pragma once
#include "../../../BasePch.h"

namespace cha
{
	/// @brief マテリアルプロパティのインタフェース
	class IMaterialProperty
	{
	public:
		IMaterialProperty() = default;
		virtual ~IMaterialProperty() = default;

		virtual void* GetPtr() = 0;
		virtual UINT GetSize() = 0;
	};
}
