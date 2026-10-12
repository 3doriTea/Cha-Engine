#include "ScoreManager.h"

ScoreManager::ScoreManager(fs::path&& _filePath) :
	FILE_PATH_{ std::move(_filePath) }
{
}

bool ScoreManager::IsUpdatedTime() const
{
	if (!current_.contains("time") || !current_["time"].is_number())
	{
		return false;  // 現在のタイムがないなら記録更新はしない
	}

	float currentTime = current_["time"].get<float>();

	if (!history_[0].contains("time") || !history_[0]["time"].is_number())
	{
		return true;  // 履歴がないなら記録更新確定
	}
	float topTime = history_[0]["time"].get<float>();

	return currentTime <= topTime;
}

void ScoreManager::Save()
{
	std::ofstream ofs{ FILE_PATH_ };
	VCU_ASSERT(ofs.is_open() && "ファイルが開けませんでした");

	json j{};

	// current_ (json型) をそのまま保存
	j["current"] = current_;

	// history_ (std::array<json, 11>) をそのままJSON配列として代入
	j["history"] = history_;

	// インデント4で整形して出力（直列化）
	ofs << j.dump(4);
}

void ScoreManager::Load()
{
	if (!fs::exists(FILE_PATH_))
	{
		// 初回起動時などファイルが存在しない場合のハンドリング
		return;
	}

	std::ifstream ifs{ FILE_PATH_ };
	if (!ifs.is_open())
	{
		VCU_LOG("ファイルない");
		return;
	}

	json j{};
	try
	{
		ifs >> j;
	}
	catch (const json::parse_error&)
	{
		VCU_LOG("読み込み失敗");
		return;
	}

	if (j.contains("current"))
	{
		current_ = j["current"];
	}

	if (j.contains("history") && j["history"].is_array())
	{
		const auto& historyArray = j["history"];
		size_t count = std::min(historyArray.size(), history_.size());

		for (size_t i = 0; i < count; ++i)
		{
			history_[i] = historyArray[i];
		}
	}
}

bool ScoreManager::IsGameCleard()
{
	// current_ がオブジェクトであり、"time" キーが存在するかチェック
	if (!current_.is_object() || !current_.contains("time"))
	{
		return false;
	}

	const auto& timeJson = current_["time"];

	// 数値型（number）であることを確認して取得
	if (timeJson.is_number())
	{
		return timeJson.get<float>() <= (60 * 2.0f);
	}

	return false;
}

const std::span<json> ScoreManager::GetHistory(const size_t _count)
{
	VCU_ASSERT((_count <= history_.size())
		&& "上位からのカウント指定はオーバーしてしまいます。");

	return std::span{ history_ }.first(_count);
}
