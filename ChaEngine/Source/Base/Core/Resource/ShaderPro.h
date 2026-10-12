#pragma once
#include "ResourceRegistryBase.h"
#include "ShaderPro/IShaderProStep.h"
#include "ShaderPro/ShaderProBuilder.h"

namespace cha
{
	/// @brief シェーダリソース
	struct ShaderProResource
	{
		std::vector<std::unique_ptr<IShaderProStep>> steps{};  // シェーダのステップ
	};

	/// @brief シェーダ読み込み設定
	struct ShaderProLoadConfig
	{
		inline ShaderProLoadConfig(
			ShaderProBuilder&& _builder) :
			builder{ std::move(_builder) }
		{
		}

		ShaderProBuilder builder;           // シェーダプロのビルダー
	};

	/// @brief シェーダリソースを格納
	class ShaderProRegistry : public ResourceRegistryBase<ShaderProResource, ShaderProLoadConfig>
	{
	public:
		ShaderProRegistry() = default;
		~ShaderProRegistry() = default;

		/// @brief 試しにシェーダをロードする
		/// @param _config 読み込み設定
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		bool TryLoad(ShaderProLoadConfig&& _config, ShaderProResource& _out) override;
	};

	template<> struct ResourceTraits<ResourceType::ShaderPro>
	{
		using DataT = ShaderProResource;
		using RegistryT = ShaderProRegistry;
		using LoadConfigT = ShaderProLoadConfig;
	};
}
