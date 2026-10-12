#pragma once
#include <Game/Game.h>
#include "INetworkMessage.h"

class MessageConverter
{
public:
	MessageConverter() = default;
	~MessageConverter() = default;

	/// @brief ネットワークメッセージをバイナリに変換
	/// @param _message ネットワークメッセージ
	/// @return バイナリ
	std::vector<char> ToBinary(INetworkMessage& _message);
	
	/// @brief バイナリをネットワークメッセージに変換
	/// @param _binary バイナリ
	/// @param _message ネットワークメッセージ
	void ToMessage(const std::span<const char> _binary, INetworkMessage& _message);

	/// @brief バイナリ状態からメッセージのプロトコルを取得する
	/// @param _binary バイナリ
	/// @return メッセージのプロトコル
	ProtocolType GetProtocolType(const std::span<const char> _binary);
};
