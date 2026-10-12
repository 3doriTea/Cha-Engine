#pragma once
#include <Game/Game.h>

/// @brief インスタンスの生存期間をログに表示するチェッククラス
class LifeChecker
{
public:
	inline LifeChecker()
	{
		VCU_LOG("Construct:{}", name_);
	}

	inline ~LifeChecker()
	{
		VCU_LOG("Destruct:{}", name_);
	}

	inline void SetName(const std::string_view _name)
	{
		name_ = _name;
	}

private:
	std::string name_{};
};
