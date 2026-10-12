#include "ComponentPools.h"
#include "../ComponentRegistry.h"
#include "ComponentPool.h"

namespace cha
{
	template<typename ComponentT>
	inline void ComponentPools::Register(const size_t _registryIndex, ComponentT* _pComponent)
	{
		const uint64_t COMPONENT_INDEX = ComponentRegistry::GetComponentIndex<ComponentT>();

		// 指定コンポーネントのプールが確保されていないなら確保する
		if (!pool_[COMPONENT_INDEX])
		{
			pool_[COMPONENT_INDEX] = std::make_shared<ComponentPool<ComponentT>>();
		}

		auto spComponentPool = std::static_pointer_cast<ComponentPool<ComponentT>>(pool_[COMPONENT_INDEX]);

		spComponentPool->Set(_registryIndex, _pComponent);
	}
	template<typename ComponentT>
	inline ComponentT& ComponentPools::At(const size_t _objectIndex)
	{
		const uint64_t COMPONENT_INDEX = ComponentRegistry::GetComponentIndex<ComponentT>();

		// 指定コンポーネントのプールが確保されていないなら確保する
		if (!pool_[COMPONENT_INDEX])
		{
			pool_[COMPONENT_INDEX] = std::make_shared<ComponentPool<ComponentT>>();
		}

		auto spComponentPool = std::static_pointer_cast<ComponentPool<ComponentT>>(pool_[COMPONENT_INDEX]);

		return spComponentPool->At(_objectIndex);
	}
}
