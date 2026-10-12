#include "JsonLoader.h"

JsonLoader::JsonLoader(const std::string_view _loadJsonFilePath) :
	loadJsonFilePath_{ _loadJsonFilePath }
{
}

bool JsonLoader::Reload()
{
	std::ifstream ifs{ loadJsonFilePath_.data() };
	if (!ifs)
	{
		VCU_LOG("Warning: JSONファイルが開けなかった。");
		return false;
	}

	json configJson{};
	ifs >> configJson;

	// 読み込んだデータを渡す
	OnLoadJson(configJson);

	return true;
}

void JsonLoader::OnLoadJson(json& _json)
{
	json_ = _json;  // 継承されない場合このクラスにデータを置く
}
