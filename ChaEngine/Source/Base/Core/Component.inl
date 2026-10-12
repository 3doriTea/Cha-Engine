#include "Component.h"
#include "ComponentPool/ComponentPools.h"
#include "PreviousComponentFlag.h"
#include "PreviousComponentRegister.h"
#include "ComponentRegistry.h"

namespace cha
{
	class ComponentPools;

	template<typename T>	
	template<typename... Args>
		requires std::constructible_from<T, Args...>
	inline Component<T>::Component(Args&&... _args)
		: T{ std::forward<Args>(_args)...}
	{
		// 指定コンポーネントフラグを立てる
		const uint64_t INDEX = ComponentRegistry::GetComponentIndex<T>();
		PreviousComponentFlag::flag_[INDEX] = true;

		// 指定のコンポーネントとしてポインタを渡す
		PreviousComponentRegister::registry_.back()[INDEX] =
			[this](cha::ComponentPools& _pools, const size_t _registryIndex)
		{
			_pools.template Register<T>(_registryIndex, static_cast<T*>(this));
		};
	}
	template<typename T>
	inline Component<T>::~Component()
	{
		// 指定コンポーネントフラグを降ろす
		const uint64_t INDEX = ComponentRegistry::GetComponentIndex<T>();
		PreviousComponentFlag::flag_[INDEX] = false;
	}
}
