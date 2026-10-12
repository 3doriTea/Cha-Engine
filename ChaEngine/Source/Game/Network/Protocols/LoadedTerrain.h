#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : LoadedTerrain
	struct LoadedTerrain : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override { return ProtocolType::LoadedTerrain; }

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(MessageRegistry& _registry) override
		{
			//_registry.AddMember();
		}

		// 以下通信メンバーたち
		std::string data{};  ///< 地形のデータ
		int32_t chunkX{};    ///< 地形のチャンクx
		int32_t chunkY{};    ///< 地形のチャンクy
	};
}
