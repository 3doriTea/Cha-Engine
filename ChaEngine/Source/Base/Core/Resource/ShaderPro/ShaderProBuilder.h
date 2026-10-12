#pragma once
#include "../../../BasePch.h"
#include "IShaderProStep.h"
#include "../Shader/ShaderCompileConfig.h"
#include "../Shader/RasterizerConfig.h"


namespace cha
{
	/// @brief シェーダプロ構築
	class ShaderProBuilder
	{
		friend class ShaderProRegistry;
	public:
		ShaderProBuilder(const fs::path& _file, const ShaderCompileConfig& _compileConfig = {});
		~ShaderProBuilder() = default;

		// ムーブを許可
		ShaderProBuilder(ShaderProBuilder&&) noexcept = default;
		ShaderProBuilder& operator=(ShaderProBuilder&&) noexcept = default;

		// コピー禁止
		ShaderProBuilder(const ShaderProBuilder&) = delete;
		ShaderProBuilder& operator=(const ShaderProBuilder&) = delete;

		/// @brief 頂点シェーダを追加する
		/// @return 自身の参照
		ShaderProBuilder& AddVertexShader(); 
		/// @brief ピクセルシェーダを追加する
		/// @param _config ラスタライザの設定
		/// @return 自身の参照
		ShaderProBuilder& AddPixelShader(const RasterizerConfig& _config = {});
		/// @brief ジオメトリシェーダを追加する
		/// @return 自身の参照
		ShaderProBuilder& AddGeometryShader();

	private:
		const fs::path FILE_;                       // コンパイルするシェーダファイル
		const ShaderCompileConfig COMPILE_CONFIG_;  // コンパイル時の設定
		std::vector<std::unique_ptr<IShaderProStep>> pSteps_{};  // ステップ
	};
}
