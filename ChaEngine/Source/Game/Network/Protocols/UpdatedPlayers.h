#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatedPlayers
	struct UpdatedPlayers : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override { return ProtocolType::UpdatedPlayers; }

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(MessageRegistry& _registry) override
		{
			_registry.AddMember(playerNames).AddMember(positions);
		}

		// 以下通信メンバーたち

		std::string playerNames{};
		SizedBuffer<float> positions{};
	};
}
