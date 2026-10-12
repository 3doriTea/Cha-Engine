#pragma once
#include "SceneBase.h"
#include "GameObjectCollection.h"

namespace cha
{
	template<typename T, typename ...Args>
	inline GameObject SceneBase::Instantiate(Args&& ..._args)
		requires std::constructible_from<T, Args...>
	{
		GameObject gameObject = pGameObjectCollection_->template Add<T>(std::forward<Args>(_args)...);

		return gameObject;
	}
}
