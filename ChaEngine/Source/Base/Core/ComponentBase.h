#pragma once
#include "ComponentRegistry.h"
#include "IComponent.h"

namespace cha
{
	/// @brief コンポーネントの基底クラス
	template<typename ComponentT, typename BaseComponentT = IComponent>
	class ComponentBase : public BaseComponentT
	{
	public:
		ComponentBase() = default;
		virtual ~ComponentBase() = default;

	private:
		// コンポーネントのインデクス
		inline static uint64_t COMPONENT_INDEX = ComponentRegistry::GetComponentIndex<ComponentT>();
	};
}
