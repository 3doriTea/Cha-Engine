#pragma once
#include <Base/Base.h>
#include "ENetTransport.h"
#include "INetworkMessage.h"

/// @brief enetでセッションを管理するマネージャー
class NetworkSessionManager : public IManager
{
public:
	/// @brief ネットワーク上の状態
	enum struct State
	{
		Disconnected,
		Connecting,
		CheckJoin,
		InLobbyNotReady,
		InLobbyWaiting,
		Playing,
		InResult,
	};

public:
	NetworkSessionManager() = default;
	~NetworkSessionManager() = default;

	/// @brief サーバーへ接続する
	void Connect();
	/// @brief 更新処理
	void Update();

	/// @brief サーバーから切断する
	void Disconnect();

	/// @brief 解放処理
	void Release() override;

	/// @brief メッセージを送信する
	/// @param _message メッセージ
	void Send(INetworkMessage& _message);

	template<typename Func>
	void Receive(Func&& _func);

	/// @brief 現在のステートを取得する
	/// @return 現在のステート
	State GetCurrentState() const { return state_; }
	/// @brief 現在のステートを変更する
	/// @param _state ステート
	void SetCurrentState(const State _state) { state_ = _state; }

private:
	ENetTransport transport_{};
	State state_{ State::Disconnected };
};

#include "NetworkSessionManager.inl"
