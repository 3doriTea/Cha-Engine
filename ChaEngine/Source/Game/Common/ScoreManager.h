#pragma once
#include <Base/Base.h>

class ScoreManager : public IManager
{
public:
	ScoreManager(fs::path&& _filePath);
	~ScoreManager() = default;

	inline void SetCurrent(const json& _json) { current_ = _json; }

	template<typename Func>
	int GetRankAndUpdateHistory(Func&& _func);

	/// @brief 現在のタイムで更新があったか
	/// @return 
	bool IsUpdatedTime() const;

	void Save();
	void Load();

	/// @brief ゲームにクリアしたか
	/// @return クリアした true / false
	bool IsGameCleard();

	/// @brief 現在のスコアを取得する
	/// @return 
	const json GetCurrentScore() { return current_; }

	/// @brief 履歴jsonの配列トップを取得する
	/// @param _count トップ何位まで取得するか
	/// @return 部分的に取得したspan
	const std::span<json> GetHistory(const size_t _count);

private:
	const fs::path FILE_PATH_;        ///< セーブファイルのパス
	json current_{};                  ///< 現在のスコア
	std::array<json, 11> history_{};  ///< 過去のスコア
};

template<typename Func>
inline int ScoreManager::GetRankAndUpdateHistory(Func&& _func)
{
	history_[10] = current_;  // 最後に入れる
	// ソートする
	std::sort(history_.begin(), history_.end(), _func);

	// ソート後の位置を二分探索で取得
	auto it = std::lower_bound(history_.begin(), history_.end(), current_);
	int index = static_cast<int>(std::distance(history_.begin(), it));
	return index;
}
