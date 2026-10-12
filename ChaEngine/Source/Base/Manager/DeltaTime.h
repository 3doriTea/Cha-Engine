#pragma once
#include "IManager.h"

namespace cha
{
	/// @brief フレーム間時間
	class DeltaTime : public IManager
	{
		friend class GameTimeManager;
	public:
		DeltaTime() = default;
		~DeltaTime() = default;

		/// @brief DeltaTimeを直接float型に変換
		inline operator float() const
		{
			return deltaTime_;
		}

	private:
		/// @brief フレーム間時間をセットする
		/// @param _deltaTime フレーム間時間(秒)
		inline void Set(const float _deltaTime) { deltaTime_ = _deltaTime; }

	private:
		float deltaTime_{};
	};
}
