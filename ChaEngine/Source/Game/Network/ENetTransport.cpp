#include "ENetTransport.h"

ENetTransport::ENetTransport()
{
}

ENetTransport::~ENetTransport()
{
	Shutdown();
}

bool ENetTransport::Initialize()
{
	if (initialized_)
	{
		return true;  // すでに初期化済み
	}

	if (enet_initialize() != 0)
	{
		return false;  // 初期化失敗
	}

	initialized_ = true;  // 初期化成功

	return true;
}

void ENetTransport::Shutdown()
{
	if (pHost_ != nullptr)
	{
		enet_host_destroy(pHost_);
		pHost_ = nullptr;
	}

	pPeer_ = nullptr;

	if (initialized_)
	{
		enet_deinitialize();
		initialized_ = false;
	}
}

bool ENetTransport::Connect(const std::string& _host, uint16_t _port, size_t _channelCount)
{
	if (!initialized_)
	{
		return false;  // まだ初期化されていないなら失敗
	}

	if (pHost_ != nullptr)
	{
		return false;  // すでに接続済みのホストがあるなら失敗
	}

	if (!CreateHost(_channelCount))
	{
		return false;  // ホストの作成に失敗
	}

	ENetAddress address{};

	if (enet_address_set_hostname(
		&address,
		_host.c_str()) != 0)
	{
		enet_host_destroy(pHost_);
		pHost_ = nullptr;

		return false;  // ホスト設定に失敗した
	}

	address.port = _port;

	pPeer_ = enet_host_connect(
		pHost_,
		&address,
		_channelCount,
		0);

	if (pPeer_ == nullptr)
	{
		enet_host_destroy(pHost_);
		pHost_ = nullptr;

		return false;
	}

	return true;
}

void ENetTransport::Disconnect(const uint32_t _data)
{
	if (pPeer_ == nullptr)
	{
		return;  // 未接続なら特に何もしない
	}

	enet_peer_disconnect(pPeer_, _data);

	enet_host_flush(pHost_);
}

bool ENetTransport::IsConnected() const
{
	if (pPeer_ == nullptr)
	{
		return false;  // そもそもつながってない
	}

	// 接続中であるかを返す
	return enet_peer_get_state(pPeer_)
		== ENET_PEER_STATE_CONNECTED;
}

bool ENetTransport::IsConnecting() const
{
	if (pPeer_ == nullptr)
	{
		return false;
	}

	const ENetPeerState state = enet_peer_get_state(pPeer_);

	return 
		state == ENET_PEER_STATE_CONNECTING ||
		state == ENET_PEER_STATE_ACKNOWLEDGING_CONNECT ||
		state == ENET_PEER_STATE_CONNECTION_PENDING;
}

std::vector<ENetTransport::Event> ENetTransport::PollEvents(const uint32_t _timeout)
{
	std::vector<Event> events{};

	if (pHost_ == nullptr)
	{
		return events;  // ホストなしなら空っぽを返す
	}

	ENetEvent event{};

	// 最初のイベント待ち
	int result = enet_host_service(
		pHost_,
		&event,
		_timeout);

	if (result < 0)
	{
		return events;  // 失敗したならそのまま返す
	}

	// 最初のイベント
	if (result > 0)
	{
		while (true)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				pPeer_ = event.peer;

				Event transportEvent;
				transportEvent.type =
					EventType::Connected;

				events.push_back(
					std::move(transportEvent)
				);

				break;
			}


			case ENET_EVENT_TYPE_DISCONNECT:
			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
			{
				Event transportEvent;
				transportEvent.type =
					EventType::Disconnected;

				events.push_back(
					std::move(transportEvent)
				);

				if (event.peer == pPeer_)
				{
					pPeer_ = nullptr;
				}

				break;
			}


			case ENET_EVENT_TYPE_RECEIVE:
			{
				Event transportEvent;

				transportEvent.type =
					EventType::Received;

				transportEvent.channel =
					event.channelID;


				if (event.packet != nullptr)
				{
					transportEvent.data.resize(
						event.packet->dataLength
					);

					if (event.packet->dataLength > 0)
					{
						std::memcpy(
							transportEvent.data.data(),
							event.packet->data,
							event.packet->dataLength
						);
					}

					// ENetが確保したPacketを解放
					enet_packet_destroy(event.packet);
				}

				events.push_back(
					std::move(transportEvent)
				);

				break;
			}


			default:
				break;
			}


			// 残っているイベントを回収
			result = enet_host_check_events(
				pHost_,
				&event);

			if (result <= 0)
			{
				break;
			}
		}
	}

	return events;

	return std::vector<Event>();
}


bool ENetTransport::Send(
	const void* data,
	const size_t size,
	const uint8_t channel,
	const ENetSendMode mode)
{
	if (pPeer_ == nullptr)
	{
		return false;
	}

	if (data == nullptr && size != 0)
	{
		return false;
	}


	uint32_t flags = ENET_PACKET_FLAG_NONE;

	switch (mode)
	{
	case ENetSendMode::Reliable:
		flags |= ENET_PACKET_FLAG_RELIABLE;
		break;
	case ENetSendMode::Unreliable:
		// ENet_PACKET_FLAG_NONE
		//
		// ENetの通常送信はUnreliable
		break;
	case ENetSendMode::Unsequenced:
		flags |= ENET_PACKET_FLAG_UNSEQUENCED;
		break;
	}

	ENetPacket* packet =
		enet_packet_create(
			data,
			size,
			flags
		);

	if (packet == nullptr)
	{
		return false;
	}


	const int result =
		enet_peer_send(
			pPeer_,
			channel,
			packet
		);

	if (result != 0)
	{
		// enet_peer_send に失敗した場合は
		// packetが解放されない可能性があるため破棄
		enet_packet_destroy(packet);

		return false;
	}

	return true;
}


bool ENetTransport::Send(
	const std::vector<char>& data,
	const uint8_t channel,
	const ENetSendMode mode)
{
	return Send(
		data.data(),
		data.size(),
		channel,
		mode
	);
}

void ENetTransport::Flush()
{
	if (pHost_ == nullptr)
	{
		return;
	}

	enet_host_flush(pHost_);
}

bool ENetTransport::CreateHost(const size_t _channelCount)
{
	if (!initialized_)
	{
		return false;  // 初期化されていないならホスト作成処理失敗
	}
	if (pHost_ != nullptr)
	{
		return false;  // ホストがまだ作成されていないならホスト作成処理失敗
	}

	pHost_ = enet_host_create(
		nullptr,
		1,
		_channelCount,
		0,
		0,
		0);

	return pHost_ != nullptr;  // ちゃんと作成できていたら成功
}
