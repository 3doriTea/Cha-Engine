#pragma once
#include "ResourceRegistryBase.h"

namespace cha
{
	/// @brief 音声リソース
	struct AudioResource
	{
		WAVEFORMATEXTENSIBLE waveFormatEx{};
		std::vector<uint8_t> buffer{};

		HashValue GetHash() const;
	};

	/// @brief 音声リソースを格納
	class AudioRegistry : public ResourceRegistryBase<AudioResource>
	{
	public:
		AudioRegistry() = default;
		~AudioRegistry() = default;

		/// @brief 試しに音声ファイルをロードする
		/// @param _file 音声ファイルパス
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		bool TryLoad(fs::path&& _file, AudioResource& _out) override;
	};

	template<> struct ResourceTraits<ResourceType::Audio>
	{
		using DataT = AudioResource;
		using RegistryT = AudioRegistry;
		using LoadConfigT = fs::path;
	};
}
