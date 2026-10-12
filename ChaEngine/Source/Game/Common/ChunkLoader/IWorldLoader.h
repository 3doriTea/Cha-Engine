#pragma once
#include <Game/Game.h>

/// @brief ワールド読み込み
class IWorldLoader
{
public:
	IWorldLoader() = default;
	virtual ~IWorldLoader() = default;

	/// @brief 指定のチャンクを読み込む
	/// @param _chunkIndex 指定のチャンクインデックス
	virtual void LoadChunk(const size_t _chunkIndex) = 0;
};
