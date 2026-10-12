#pragma once
#include <Base/Base.h>

/// @brief JSONを読み込むクラス 基底クラスとしても使える
class JsonLoader
{
public:
	/// @brief JSONファイルの読み込みを行う
	/// @param _loadJsonFilePath JSONファイルのファイルパス
	JsonLoader(const std::string_view _loadJsonFilePath);
	virtual ~JsonLoader() = default;

	/// @brief JSONファイルの読み込みを行う
	/// @return 読み込みに成功 true / false
	virtual bool Reload();

	/// @brief JSONLoaderのデータにアクセス
	/// @return 読み込んだJSONの参照
	inline json& JsonData() { return json_; }

protected:
	/// @brief JSONファイルが読み込まれたときの処理
	/// @param _json 読み込んだJSON
	virtual void OnLoadJson(json& _json);

private:
	std::string_view loadJsonFilePath_{};  // 読み込むJSONファイルのパス
	json json_{};  // 無継承で使う場合のjsonデータ
};
