
#include "GameTimeManager.h"
#include "DeltaTime.h"
#include <algorithm>

cha::GameTimeManager::GameTimeManager(const uint32_t _targetFPS, DeltaTime& _deltaTime) :
	targetFPS_{ _targetFPS },
	deltaTime_{ _deltaTime }
{
	QueryPerformanceCounter(&performanceCounterPrev_);
}

bool cha::GameTimeManager::Update()
{
	if (QueryPerformanceFrequency(&performanceFrequency_) == FALSE)
	{
		VCU_ASSERT(false && "CPU周波数取得失敗");
		return false;  // 失敗
	}

	if (QueryPerformanceCounter(&performanceCounter_) == FALSE)
	{
		VCU_ASSERT(false && "高精度カウンタ値取得失敗");
		return false;  // 失敗
	}

	const int64_t DIFF{ performanceCounter_.QuadPart - performanceCounterPrev_.QuadPart };

	// フレーム更新タイミングか
	const bool IS_FRAME_DUE{ DIFF * targetFPS_ > performanceFrequency_.QuadPart };

	if (IS_FRAME_DUE)
	{
		// フレーム間時間を求める
		deltaTime_.Set(std::clamp(static_cast<float>(DIFF) / performanceFrequency_.QuadPart, 0.0f, 0.1f));
		// 戻しておく
		performanceCounterPrev_.QuadPart = performanceCounter_.QuadPart;
	}

	// フレーム更新タイミングかどうかを返す
	return IS_FRAME_DUE;
}
