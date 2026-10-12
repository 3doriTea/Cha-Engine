#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : LobbyMembers
	struct LobbyMembers : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override { return ProtocolType::LobbyMembers; }

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(MessageRegistry& _registry) override
		{
			_registry.AddMember(names);
		}

		// 以下通信メンバーたち
		std::string names{};
	};
}
