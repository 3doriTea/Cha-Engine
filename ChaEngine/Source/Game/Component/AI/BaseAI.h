#pragma once
#include <Game/Game.h>
#include "IAI.h"

/// @brief AIのベース
/// @tparam AIT 継承したAI構造体
template<typename AIT>
struct BaseAI : IAI, Component<AIT>
{
	virtual ~BaseAI() = default;
};
