
#include "TopologicalSort.h"

void Utility::Topological::Sort(const std::vector<std::vector<size_t>>& _input, std::vector<size_t>& _result)
{
	// REF: https://zenn.dev/reputeless/books/standard-cpp-for-competitive-programming/viewer/topological-sort

	std::vector<int> indegrees(_input.size());

	// 各要素の入り次数をカウントしていく
	for (const auto& v : _input)
	{
		for (const auto& to : v)
		{
			indegrees[to]++;
		}
	}

	// 入り次数のQueue
	std::queue<int> q{};

	// 入り次数0の頂点をキューに追加しまくる
	for (int i = 0; i < static_cast<int>(_input.size()); i++)
	{
		if (indegrees[i] == 0)
		{
			q.push(i);
		}
	}

	// 全参照がほどけるまで繰り返す
	while (!q.empty())
	{
		// キューから取り出す
		const int from = q.front();
		q.pop();

		_result.push_back(from);

		// 入り次数0になった頂点からの各辺に関し入り次数を減らす
		for (const auto& to : _input[from])
		{
			indegrees[to]--;
			// 減らした結果0になったらキューに入れる
			if (indegrees[to] == 0)
			{
				q.push(static_cast<int>(to));
			}
		}
	}

	// 閉路が存在するなら諦める
	VCU_ASSERT(_result.size() == _input.size()
		&& "閉路が存在する");
}
