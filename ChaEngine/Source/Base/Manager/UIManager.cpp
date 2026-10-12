#include "UIManager.h"
#include "D2DManager.h"
#include "../ManagerRegistry.h"

bool cha::UIManager::Update()
{
	// レイヤーでソート(安定ソート)
	std::stable_sort(renderQueue_.begin(), renderQueue_.end(),
		[](const UIRenderElement& _left, const UIRenderElement& _right)
	{
		return _left.order < _right.order;
	});

	// D2D の Begin/End を要するものは分岐して処理する
	bool d2dBegun = false;
	for (auto& elem : renderQueue_)
	{
		if (elem.isD2D)
		{
			if (!d2dBegun)
			{
				ManagerRegistry::Ref<D2DManager>().Begin();
				d2dBegun = true;
			}
			if (elem.func) elem.func();
		}
		else
		{
			if (d2dBegun)
			{
				ManagerRegistry::Ref<D2DManager>().End();
				d2dBegun = false;
			}
			if (elem.func) elem.func();
		}
	}

	if (d2dBegun)
	{
		ManagerRegistry::Ref<D2DManager>().End();
	}

	// 描画しきったら消す
	renderQueue_.clear();

	return true;  // 更新成功
}

void cha::UIManager::AddRenderer(std::function<void()>&& _func, const UILayerOrder _order, const bool _isD2D)
{
	renderQueue_.push_back({ _order, std::move(_func), _isD2D });
}
