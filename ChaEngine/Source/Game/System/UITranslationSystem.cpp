
#include "UITranslationSystem.h"
#include "../Component/UI/UITransform.h"
#include <Game/Utility/Algorithm/TopologicalSort.h>

void UITranslationSystem::Update()
{
	auto view = GetView<UITransform>();

	checked_.clear();
	calculationQueue_.clear();

	std::unordered_map<uint64_t, size_t> idToIndex{};
	std::vector<bool> ignoreFlags(view.GetSize(), true);
	std::vector<std::vector<size_t>> refGraph(view.GetSize(), std::vector<size_t>{});

	view.ForEachWithIndex([&idToIndex, &ignoreFlags](const size_t _index, IObject& _object, UITransform& _transform)
	{
		using namespace DirectX;

		ignoreFlags[_index] = false;  // 無視しない
		idToIndex[_object.GetId()] = _index;

		// UI行列を計算する
		_transform.uiWorld = _transform.uiLocal = XMMatrixIdentity()
			* DirectX::XMMatrixScaling(_transform.scale.x, _transform.scale.y, 1.0f)
			* DirectX::XMMatrixTranslation(_transform.position.x, _transform.position.y, 0.0f);
	});

	// 依存関係グラフの構築
	view.ForEachWithIndex([&idToIndex, &refGraph](const size_t _index, IObject& _object, UITransform& _transform)
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
		view.AtFast(index, [&idToIndex, &view](IObject& _objectSelf, UITransform& _transformSelf)
		{
			if (_transformSelf.parent)
			{
				uint64_t parentId = _transformSelf.parent.GetId();
				if (idToIndex.contains(parentId))
				{
					size_t parentIndex = idToIndex[parentId];

					view.AtFast(parentIndex, [&_transformSelf](IObject& _objectParent, UITransform& _transformParent)
					{
						using namespace DirectX;
						_transformSelf.uiWorld = _transformSelf.uiLocal * _transformParent.uiWorld;
					});
				}
			}
		});
	}
}
