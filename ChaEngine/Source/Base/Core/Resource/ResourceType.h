#pragma once
#include "../../BasePch.h"

namespace cha
{
	/// @brief リソースの種類
	enum struct ResourceType : uint32_t
	{
		Shader,     ///< シェーダ
		Surface,    ///< 面
		Model,      ///< 3Dモデル
		Texture,    ///< テクスチャ
		Audio,      ///< 音声
		Material,   ///< コンスタントバッファへ送る素材データ
		ShaderPro,  ///< 高自由度なシェーダ
	};
}
