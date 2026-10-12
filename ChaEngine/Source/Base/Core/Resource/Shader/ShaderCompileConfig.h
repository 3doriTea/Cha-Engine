#pragma once
#include "../../../BasePch.h"

namespace cha
{
	/// @brief シェーダコンパイル時の設定
	struct ShaderCompileConfig
	{
		std::string vertexEntryName{ "VS" };      // 頂点シェーダエントリポイント名
		std::string vertexVersion  { "vs_5_0" };  // 頂点シェーダのバージョン
		std::string pixelEntryName { "PS" };      // ピクセルシェーダのエントリポイント名
		std::string pixelVersion   { "ps_5_0" };  // ピクセルシェーダのバージョン
		std::string geometryEntryName{ "GS" };    // ジオメトリシェーダのエントリポイント名
		std::string geometryVersion{ "gs_5_0" };  // ジオメトリシェーダのバージョン
		ID3DInclude* pInclude      { D3D_COMPILE_STANDARD_FILE_INCLUDE };
		UINT flags                 { D3DCOMPILE_ENABLE_STRICTNESS };
	};
}
