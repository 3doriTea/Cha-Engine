#pragma once
#include "ProtocolType.h"
#include "MessageRegistry.h"
#include <Game/Common/Utility/SizedBuffer.h>
#include "ENetSendMode.h"

/// @brief ネットワークで流れるメッセージのインタフェース
class INetworkMessage
{
public:
	INetworkMessage() = default;
	virtual ~INetworkMessage() = default;

	/// @brief プロトコルの種類を取得する
	/// @return プロトコルの種類
	virtual ProtocolType GetProtocolType() const = 0;

	virtual void LoadMembers(MessageRegistry& _registry) = 0;

	/// @brief 送信するチャンネル番号を取得する
	/// @return チャンネル番号
	virtual uint8_t GetChannel() const { return 0; }

	/// @brief 送信モードを取得する
	/// @return 送信モード
	virtual ENetSendMode GetSendMode() const { return ENetSendMode::Reliable; }
};
