#include "pch\pch.h"
#include "StageLoader.h"


bool StageLoader::TryLoad(const fs::path& _filePath)
{
	std::ifstream ifs{ _filePath, std::ios::binary };

	if (!ifs && !ifs.is_open())
	{
		VCU_ASSERT(false && "ファイルが見つからなかった");
		return false;
	}

	ifs.seekg(0, std::ios::end);
	std::streamsize size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (!ifs.read(buffer.data(), size))
	{
		VCU_ASSERT(false && "ファイルの読み込みに失敗");
		return false;
	}



	//ifs.read();

	return true;
}

bool StageLoader::TrySave(const fs::path& _filePath)
{
	return false;
}
