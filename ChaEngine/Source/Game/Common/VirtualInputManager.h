#pragma once
#include <Base/Base.h>
#include <Game/Utility/File/JsonLoader.h>
#include <winsock2.h>
#include <ws2tcpip.h>

class VirtualInputManager : public IManager
{
public:
	VirtualInputManager() = default;
	~VirtualInputManager() = default;

	bool Initialize();
	void Update();
	void Release() override;

	/// @brief ボタンが押されているか取得
	/// @return 押されている true / false
	bool OnButton();
	/// @brief ボタンが押された瞬間か取得
	/// @return 押されている true / false
	bool OnButtonDown();
	/// @brief ボタンが離された瞬間か取得
	/// @return 押されている true / false
	bool OnButtonUp();

private:
	void TryConnect();
	void CheckConnecting();
	void CloseSocket();

private:
	bool currentButton_{ false };   ///< 現在のボタンのon / offステート
	bool previousButton_{ false };  ///< 前回のボタンのon / offステート
	JsonLoader connectionConfig_{ "./ConnectionConfig.json" };  ///< 接続設定JSON

	SOCKET connectSocket_ = INVALID_SOCKET;
	bool isConnecting_ = false;
	std::string serverIp_;
	uint16_t serverPort_ = 3333;
};
