#pragma once
#include "../BasePch.h"
#include "IManager.h"

namespace cha
{
	/// @brief スクリーンサイズ
	class ScreenSize : public IManager
	{
		friend class GameTimeManager;
	public:
		inline ScreenSize(const Vec2 _clientSize) :
			CLIENT_SIZE{ _clientSize }
		{}
		~ScreenSize() = default;

		/// @brief ScreenSizeを直接Vec2型に変換
		inline operator Vec2() const
		{
			return CLIENT_SIZE;
		}
	private:
		const Vec2 CLIENT_SIZE;
	};
}
