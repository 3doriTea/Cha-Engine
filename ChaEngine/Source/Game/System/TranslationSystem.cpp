
#include "TranslationSystem.h"
#include <Game/Utility/Algorithm/TopologicalSort.h>

void TranslationSystem::Update()
{
	auto view = GetView<Transform>();

	checked_.clear();
	calculationQueue_.clear();

	std::unordered_map<uint64_t, size_t> idToIndex{};
	std::vector<bool> ignoreFlags(view.GetSize(), true);
	std::vector<std::vector<size_t>> refGraph(view.GetSize(), std::vector<size_t>{});

	view.ForEachWithIndex([&idToIndex, &ignoreFlags](const size_t _index, IObject& _object, Transform& _transform)
	{
		using namespace DirectX;

		ignoreFlags[_index] = false;  // 無視しない
		idToIndex[_object.GetId()] = _index;

		// 法線方向の行列を計算する
		_transform.normal = _transform.localNormal = XMMatrixIdentity()
			* XMMatrixRotationQuaternion(_transform.rotation);

		// 座標変換行列を計算する
		_transform.world = _transform.local = XMMatrixIdentity()
			* XMMatrixScalingFromVector(_transform.scale)
			* XMMatrixRotationQuaternion(_transform.rotation)
			* XMMatrixTranslationFromVector(_transform.position);
	});

	// 依存関係グラフの構築
	view.ForEachWithIndex([&idToIndex, &refGraph](const size_t _index, IObject& _object, Transform& _transform)
	{
		if (_transform.parent)
		{
			uint64_t parentId = _transform.parent.GetId();
			// 親がView内に存在するかチェック
			if (idToIndex.contains(parentId))
			{
				size_t parentIndex = idToIndex[parentId];
				refGraph[parentIndex].push_back(_index);
			}
		}
	});

	// トポロジカルソートして参照関係に問題ない順番にする
	std::vector<size_t> sortedIndices{};
	Utility::Topological::Sort(refGraph, sortedIndices);

	for (const auto index : sortedIndices)
	{
		if (ignoreFlags[index])
		{
			continue;
		}
		view.AtFast(index, [&idToIndex, &view](IObject& _objectSelf, Transform& _transformSelf)
		{
			if (_transformSelf.parent)
			{
				uint64_t parentId = _transformSelf.parent.GetId();
				if (idToIndex.contains(parentId))
				{
					size_t parentIndex = idToIndex[parentId];

					view.AtFast(parentIndex, [&_transformSelf](IObject& _objectParent, Transform& _transformParent)
					{
						using namespace DirectX;
						_transformSelf.world = _transformSelf.local * _transformParent.world;
					});
				}
			}
		});
	}
}
