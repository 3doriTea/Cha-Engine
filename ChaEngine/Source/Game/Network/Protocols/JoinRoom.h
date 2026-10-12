#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : 部屋に参加するとき
	struct JoinRoom : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override { return ProtocolType::JoinRoom; }

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(MessageRegistry& _registry) override
		{
			_registry.AddMember(roomName);
		}
		
		// 以下通信メンバーたち
		std::string roomName{};  ///< 参加したい部屋名
	};
}
