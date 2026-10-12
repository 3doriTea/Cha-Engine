#include "pch\pch.h"
#include "NetworkSessionManager.h"
#include "MessageConverter.h"

void NetworkSessionManager::Connect()
{
	bool succeed = transport_.Initialize();
	VCU_ASSERT(succeed && "ENetTransportの初期化に失敗");

	static const fs::path CONNECTION_JSON_FILE{ "ConnectionConfig.json" };

	json connectionJson{};
	std::ifstream ifs{ CONNECTION_JSON_FILE };
	
	VCU_ASSERT((ifs && ifs.is_open()) && "接続設定のjsonファイルが開けなかった");

	ifs >> connectionJson;

	std::string hostName{ connectionJson.value("ip", "127.0.0.1") };
	uint16_t portNumber{ connectionJson.value("port", 43838ui16) };

	succeed = transport_.Connect(hostName, portNumber);
	VCU_ASSERT(succeed && "サーバーへの接続に失敗");
}

void NetworkSessionManager::Update()
{
}

void NetworkSessionManager::Disconnect()
{
	transport_.Disconnect();
}

void NetworkSessionManager::Release()
{
	transport_.Shutdown();
}

void NetworkSessionManager::Send(INetworkMessage& _message)
{
	MessageConverter converter{};

	std::vector<char> binary = converter.ToBinary(_message);

	transport_.Send(binary, _message.GetChannel(), _message.GetSendMode());
}
