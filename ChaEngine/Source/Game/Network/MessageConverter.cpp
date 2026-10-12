#include "pch\pch.h"
#include "MessageConverter.h"
#include "MessageRegistry.h"

std::vector<char> MessageConverter::ToBinary(INetworkMessage& _message)
{
	std::vector<char> buffer{};

	buffer.push_back(static_cast<uint8_t>(_message.GetProtocolType()));
	size_t pos{ 1 };
	
	MessageRegistry registry{};
	_message.LoadMembers(registry);

	for (auto& value : registry.members)
	{
		std::visit([&buffer](auto* _arg)
		{
			using T = std::decay_t<decltype(*_arg)>;

			// 指定したサイズ分をバッファに書き込む
			auto writeToBuffer = [&buffer, &_arg](const size_t _vSize)
			{
				size_t oldBufferSize = buffer.size();
				buffer.resize(buffer.size() + _vSize, 0x00);
				std::memcpy(buffer.data() + oldBufferSize, _arg, _vSize);
			};

			if constexpr (std::is_same_v<T, std::string>)
			{
				// _arg 自体が std::string&
				const size_t vSize = _arg->size() + 1; // null文字含める

				size_t oldBufferSize = buffer.size();
				buffer.resize(oldBufferSize + vSize, 0x00);

				// c_str() を使って null文字まで安全にコピー
				std::memcpy(buffer.data() + oldBufferSize, _arg->c_str(), vSize);
			}
			else if constexpr (std::is_base_of_v<ISizedBuffer, T>)
			{
				// 先頭にデータサイズ (size_t) を付与して追記
				const int32_t payloadSize = static_cast<int32_t>(_arg->ByteSize());
				const size_t totalSize = payloadSize + sizeof(int32_t);

				size_t oldBufferSize = buffer.size();
				buffer.resize(oldBufferSize + totalSize, 0x00);

				// 1. サイズ情報の書き込み
				std::memcpy(buffer.data() + oldBufferSize, &payloadSize, sizeof(int32_t));

				// 2. 実データの書き込み
				if (payloadSize > 0)
				{
					std::memcpy(buffer.data() + oldBufferSize + sizeof(int32_t), _arg->Data(), payloadSize);
				}
			}
			else
			{
				writeToBuffer(sizeof(T));
			}
		},
		value);
	}

	return buffer;
}

void MessageConverter::ToMessage(const std::span<const char> _binary, INetworkMessage& _message)
{
	VCU_ASSERT((_message.GetProtocolType() == GetProtocolType(_binary))
		&& "プロトコルタイプのミスマッチが起きている");

	MessageRegistry registry{};
	_message.LoadMembers(registry);

	size_t pos{ 1 };

	for (auto& value : registry.members)
	{
		std::visit([&_binary, &pos](auto* _arg)
		{
			using T = std::decay_t<decltype(*_arg)>;

			// 指定したサイズ分をバッファに書き込む
			auto readFromBuffer = [&_binary, &pos, &_arg](const size_t _vSize)
			{
				VCU_ASSERT(pos + _vSize <= _binary.size());
				std::memcpy(_arg, &_binary[pos], _vSize);
				pos += _vSize;
			};

			if constexpr (std::is_same_v<T, std::string>)
			{
				auto itr = std::find(_binary.begin() + pos, _binary.end(), '\0');
				//VCU_ASSERT(_binary.end() != itr);

				// null文字までの長さを計算して設定
				size_t strLen = std::distance(_binary.begin() + pos, itr);
				_arg->assign(&_binary[pos], strLen);

				// pos を null文字の次まで進める
				pos += strLen + 1;
			}
			else if constexpr (std::is_base_of_v<ISizedBuffer, T>)
			{
				// 1. サイズ情報を読み込む
				const int32_t headerSize = static_cast<int32_t>(sizeof(int32_t));
				const bool isHeaderValid = (pos + headerSize) <= _binary.size();
				VCU_ASSERT(isHeaderValid);

				int32_t payloadSize = 0;
				std::memcpy(&payloadSize, &_binary[pos], headerSize);
				pos += headerSize;

				VCU_ASSERT((payloadSize >= 0) && "データサイズが不正（負の値）です");

				// 2. 実データを読み込む
				const size_t requiredBytes = static_cast<size_t>(payloadSize);

				_arg->ResizeBytes(requiredBytes);
				const bool isCapacityEnough = _arg->ByteSize() >= requiredBytes;
				VCU_ASSERT(isCapacityEnough && "受けるバッファの容量が不足しています");
	
				if (requiredBytes > 0)
				{
					std::memcpy(_arg->Data(), &_binary[pos], requiredBytes);
					pos += requiredBytes;
				}
			}
			else
			{
				readFromBuffer(sizeof(T));
			}
		}, value);
	}
}

ProtocolType MessageConverter::GetProtocolType(const std::span<const char> _binary)
{
	if (_binary.size() == 0)
	{
		return ProtocolType::Idk;  // バイナリが空っぽ
	}

	const char HEAD_BYTE = _binary.front();
	return static_cast<ProtocolType>(HEAD_BYTE);
}
