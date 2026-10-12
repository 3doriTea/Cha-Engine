#include "DependencyResolver.h"

std::vector<uint64_t> cha::DependencyResolver::Resolve()
{
	// TODO: デッドロック結果を出力する
	deadlocked_.clear();  // デッドロック結果用

	// 入次数を管理する配列
	std::vector<uint32_t> indegrees_ = std::vector<uint32_t>(links_.size(), 0);

	for (const auto&[self, refers] : links_)
	{
		for (const uint64_t& to : refers)
		{
			indegrees_[to]++;  // 入次数加算
		}
	}

	// 入次数 0 の要素を管理する
	std::queue<uint64_t> queue_ = {};

	// 現時点での入次数 0 の要素をEnqueueしていく
	for (int i = 0; i < links_.size(); i++)
	{
		if (indegrees_[i] == 0)
		{
			queue_.push(i);
		}
	}

	// 結果
	std::vector<uint64_t> result_ = {};

	// キューが空っぽになるまで処理
	while (queue_.empty() == false)
	{
		// 入次数 0 の要素を1つ取り出す
		uint64_t from{ queue_.front() };
		queue_.pop();

		result_.push_back(from);

		// 取り出した要素から参照される要素の入次数を減らしていく
		for (const uint64_t to : links_[from].refers)
		{
			indegrees_[to]--;
			// 0 になったらキューに追加
			if ( indegrees_[to] <= 0 )
			{
				queue_.push(to);
			}
		}
	}

	if (result_.size() == links_.size())
	{
		return result_;
	}
	// すべての要素を処理できていない == 閉路が存在する

	// 閉路 / 閉路に依存している要素を列挙
	for (int i = 0; i < indegrees_.size(); i++)
	{
		if (indegrees_[i] > 0)
		{
			deadlocked_.push_back(i);
		}
	}
	return std::vector<uint64_t>{};
}
