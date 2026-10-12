#pragma once
#include "IManager.h"

namespace cha
{
	class DeltaTime;

	/// @brief ゲーム内時間を扱う
	class GameTimeManager : public IManager
	{
	public:
		GameTimeManager(const uint32_t _targetFPS, DeltaTime& _deltaTime);

		bool Update();

	private:
		LARGE_INTEGER performanceFrequency_{};    // CPU周波数
		LARGE_INTEGER performanceCounter_{};      // 高精度カウンタ値
		LARGE_INTEGER performanceCounterPrev_{};  // 前回の高精度カウンタ値

		uint32_t targetFPS_{};  // 目指すFPS
		DeltaTime& deltaTime_;  // フレーム間時間格納用マネージャ
	};
}
