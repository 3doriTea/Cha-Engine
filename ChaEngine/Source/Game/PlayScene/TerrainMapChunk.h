#pragma once
#include "TerrainMapResolution.h"

/// @brief 地形高さのチャンクマップ
using TerrainMapChunk = std::array<std::array<uint16_t, TERRAIN_MAP_VERTICES_WIDTH>, TERRAIN_MAP_VERTICES_WIDTH>;
