#pragma once
#include "../INetworkMessage.h"

namespace Protocol
{
	/// @brief プロトコル : UpdatePlayer
	struct UpdatePlayer : INetworkMessage
	{
		/// @brief プロトコルの種類を取得する
		/// @return プロトコルの種類
		ProtocolType GetProtocolType() const override { return ProtocolType::UpdatePlayer; }

		/// @brief メンバーをロードする
		/// @param _registry メンバーを登録するやつ
		void LoadMembers(MessageRegistry& _registry) override
		{
			_registry
				.AddMember(positionX)
				.AddMember(positionY)
				.AddMember(positionZ);
		}

		uint8_t GetChannel() const override { return 1; }
		ENetSendMode GetSendMode() const override { return ENetSendMode::Unreliable; }

		// 以下通信メンバーたち
		float positionX{};
		float positionY{};
		float positionZ{};
	};
}
