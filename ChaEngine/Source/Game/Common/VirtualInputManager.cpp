#include "pch\pch.h"
#include "VirtualInputManager.h"

#pragma comment(lib, "ws2_32.lib")

bool VirtualInputManager::Initialize()
{
	bool succeed = connectionConfig_.Reload();
	
	if (!succeed)
	{
		return true;
	}
	
	const json& json = connectionConfig_.JsonData();

	serverIp_ = json.value("ip", "127.0.0.1");
	serverPort_ = json.value("port", 3333);

	WSADATA wsaData{};
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{
		VCU_ASSERT(false && "WSAStartup 失敗");
		return false;
	}

	connectSocket_ = INVALID_SOCKET;
	isConnecting_ = false;
	currentButton_ = false;
	previousButton_ = false;

	return true;
}

void VirtualInputManager::Update()
{
	// 前フレームのボタン状態を更新
	previousButton_ = currentButton_;

	// 1. 未接続の場合、再接続処理を試行
	if (connectSocket_ == INVALID_SOCKET)
	{
		TryConnect();
	}
	else if (isConnecting_)
	{
		// 非同期接続の完了チェック
		CheckConnecting();
	}

	// 2. 接続確立中の場合、データの受信チェック
	if (connectSocket_ != INVALID_SOCKET && !isConnecting_)
	{
		char buffer[64]{};
		int bytesReceived = recv(connectSocket_, buffer, sizeof(buffer) - 1, 0);

		if (bytesReceived > 0)
		{
			buffer[bytesReceived] = '\0';
			std::string msg(buffer);

			// 送信文字列に応じてボタン状態を更新 ("ON" / "OFF")
			if (msg.find("ON") != std::string::npos)
			{
				currentButton_ = true;
			}
			else if (msg.find("OFF") != std::string::npos)
			{
				currentButton_ = false;
			}
		}
		else if (bytesReceived == 0)
		{
			// サーバー（ラズパイ）側から切断された場合
			CloseSocket();
		}
		else
		{
			int err = WSAGetLastError();
			// WSAEWOULDBLOCK は受信データなしを意味する正常な挙動
			if (err != WSAEWOULDBLOCK)
			{
				// 通信エラー発生時はソケットを閉じて次回Updateで再接続させる
				CloseSocket();
			}
		}
	}
}

void VirtualInputManager::TryConnect()
{
	// 一定間隔（例: 毎フレームだと負荷が高いため簡易タイマー等を挟んでも良い）で接続試行
	connectSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket_ == INVALID_SOCKET)
	{
		return;
	}

	// ソケットをノンブロッキングモードに設定
	u_long mode = 1;
	ioctlsocket(connectSocket_, FIONBIO, &mode);

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort_);
	inet_pton(AF_INET, serverIp_.c_str(), &serverAddr.sin_addr);

	int result = connect(connectSocket_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
		{
			// 非同期接続開始（接続待ち状態）
			isConnecting_ = true;
		}
		else
		{
			// 接続失敗
			CloseSocket();
		}
	}
	else
	{
		// 即時接続成功
		isConnecting_ = false;
	}
}

void VirtualInputManager::CheckConnecting()
{
	fd_set writeSet, errSet;
	FD_ZERO(&writeSet);
	FD_ZERO(&errSet);
	FD_SET(connectSocket_, &writeSet);
	FD_SET(connectSocket_, &errSet);

	timeval timeout{ 0, 0 }; // 即時リターン
	int selectResult = select(0, nullptr, &writeSet, &errSet, &timeout);

	if (selectResult > 0)
	{
		if (FD_ISSET(connectSocket_, &writeSet))
		{
			// 接続完了！
			isConnecting_ = false;
		}
		else if (FD_ISSET(connectSocket_, &errSet))
		{
			// 接続失敗
			CloseSocket();
		}
	}
}

void VirtualInputManager::CloseSocket()
{
	if (connectSocket_ != INVALID_SOCKET)
	{
		closesocket(connectSocket_);
		connectSocket_ = INVALID_SOCKET;
	}
	isConnecting_ = false;
	currentButton_ = false;
}

void VirtualInputManager::Release()
{
	CloseSocket();
	WSACleanup();
}

bool VirtualInputManager::OnButton()
{
	return currentButton_;
}

bool VirtualInputManager::OnButtonDown()
{
	return currentButton_ && !previousButton_;
}

bool VirtualInputManager::OnButtonUp()
{
	return !currentButton_ && previousButton_;
}