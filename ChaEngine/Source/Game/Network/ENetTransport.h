#pragma once
#include <Game/Game.h>
#include <enet/enet.h>
#include "ENetSendMode.h"


class ENetTransport : IManager
{
public:
	/// @brief イベントの種類
	enum struct EventType
	{
		Connected,     ///< 接続した
		Disconnected,  ///< 切断した
		Received,      ///< 受信した
	};

	/// @brief イベント
	struct Event
	{
		EventType type{};             ///< イベントの種類
		uint8_t channel{};            ///< Receivedの場合のみ使用するチャンネル番号
		std::vector<char> data{};  ///< Receivedの場合のみ使用する受信データ
	};

public:
	/// @brief コンストラクタ
	ENetTransport();
	/// @brief デストラクタ
	~ENetTransport();

	// コピー禁止
	ENetTransport(const ENetTransport&) = delete;
	// コピー禁止
	ENetTransport& operator=(const ENetTransport&) = delete;

	/// @brief 初期化する
	/// @return 初期化に成功 true / false
	bool Initialize();
	/// @brief 終了する
	void Shutdown();

	/// @brief 接続する
	/// @param _host ホスト名
	/// @param _port ポート番号
	/// @param _channelCount 使用チャンネル数
	/// @return 接続処理自体が成功したか true / false
	bool Connect(
		const std::string& _host,
		const uint16_t _port,
		const size_t _channelCount = 2);

	/// @brief 切断する
	/// @param _data 切断データ
	void Disconnect(const uint32_t _data = 0);

	/// @brief 接続されたか
	/// @return 接続された true / false
	bool IsConnected() const;
	/// @brief 接続中であるか
	/// @return 接続中である true / false
	bool IsConnecting() const;

	/// @brief 受信したイベントを取得する
	std::vector<Event> PollEvents(const uint32_t _timeout = 0);

	bool Send(
		const void* _data,
		const size_t _size,
		const uint8_t _channel = 0,
		const ENetSendMode _mode = ENetSendMode::Reliable);

	bool Send(
		const std::vector<char>& _data,
		const uint8_t _channel = 0,
		const ENetSendMode _mode = ENetSendMode::Reliable);

	void Flush();

	/// @brief ピアを取得
	/// @return ピア
	ENetPeer* GetPeer() const { return pPeer_; }

	/// @brief ホストを取得
	/// @return ホスト
	ENetHost* GetHost() const { return pHost_; }

private:
	bool CreateHost(const size_t _channelCount);

private:
	ENetHost* pHost_{ nullptr };  ///< ホスト
	ENetPeer* pPeer_{ nullptr };  ///< ピア
	bool initialized_{ false };   ///< 初期化済みか true / false
};
