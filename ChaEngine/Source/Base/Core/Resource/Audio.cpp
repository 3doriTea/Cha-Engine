
#include "Audio.h"

bool cha::AudioRegistry::TryLoad(fs::path&& _file, AudioResource& _out)
{
	std::ifstream ifs{ _file, std::ios::binary };
	if (!ifs)
	{
		return false;
	}

	std::array<char, 4> chunkId{};
	uint32_t chunkSize{};

	ifs.read(chunkId.data(), chunkId.size());
	ifs.seekg(8, std::ios::cur);  // sizeと"WAVE"識別子をシーク

	while (
		ifs.read(chunkId.data(), chunkId.size())
		&& ifs.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)))
	{
		if (strncmp(chunkId.data(), "fmt ", 4) == 0)
		{
			ifs.read(reinterpret_cast<char*>(&_out.waveFormatEx), std::min<size_t>(chunkSize, sizeof(_out.waveFormatEx)));
			if (chunkSize > sizeof(_out.waveFormatEx))
			{
				ifs.seekg(chunkSize - sizeof(_out.waveFormatEx), std::ios::cur);
			}
		}
		else if (strncmp(chunkId.data(), "data", 4) == 0)
		{
			_out.buffer.resize(chunkSize);
			ifs.read(reinterpret_cast<char*>(_out.buffer.data()), chunkSize);
		}
		else {
			ifs.seekg(chunkSize, std::ios::cur);
		}
	}

	return !_out.buffer.empty();
}

cha::HashValue cha::AudioResource::GetHash() const
{
	HashValue seed{ 0 };

	HashCombine(seed, waveFormatEx.Format.wFormatTag);
	HashCombine(seed, waveFormatEx.Format.nChannels);
	HashCombine(seed, waveFormatEx.Format.nSamplesPerSec);
	HashCombine(seed, waveFormatEx.Format.wBitsPerSample);

	// 拡張フォーマットの場合のみ追加メンバを計算
	if (waveFormatEx.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		HashCombine(seed, waveFormatEx.Samples.wValidBitsPerSample);
		HashCombine(seed, waveFormatEx.dwChannelMask);
		// GUID (SubFormat) のハッシュ化
		// Data1, Data2, Data3, Data4をまとめて扱う
		HashCombine(seed, waveFormatEx.SubFormat.Data1);
	}

	return seed;
}
