#pragma once
#include <cstdint>

namespace cha
{
	/// @brief 無効なハンドル
	inline static constexpr uint32_t INVALID_HANDLE{ 0 };

	/// @brief シェーダハンドル
	using ShaderHandle = uint32_t;

	/// @brief テクスチャハンドル
	using TextureHandle = uint32_t;

	/// @brief マテリアルハンドル
	using MaterialHandle = uint32_t;

	/// @brief モデルハンドル
	using ModelHandle = uint32_t;

	/// @brief メッシュのハンドル
	using MeshHandle = uint32_t;

	/// @brief スクリプトのハンドル
	using ScriptHandle = uint32_t;

	/// @brief 不明なハンドル
	using Handle = uint32_t;
}
