#include "View.h"
#include "../ComponentRegistry.h"
#include "../IObject.h"
#include "../GameObjectCollection.h"

template<typename ...ComponentsT>
inline cha::View<ComponentsT...>::View(GameObjectCollection& _collection) :
	collection_{ _collection }
{
}

template<typename ...ComponentsT>
template<typename Func>
inline void cha::View<ComponentsT...>::ForEach(Func&& _func)
{
	ComponentFlag flagMask{};
	GetComponentFlag<ComponentsT...>(flagMask);

	collection_.ForEach([this, flagMask, &_func](IObject& _object, ComponentFlag _flag)
	{
		if ((_flag & flagMask) == flagMask)
		{
			_func(_object, collection_.template GetComponent<ComponentsT>(_object.GetId())...);
		}
	});
}

template<typename ...ComponentsT>
template<typename Func>
inline void cha::View<ComponentsT...>::ForEachWithIndex(Func&& _func)
{
	ComponentFlag flagMask{};
	GetComponentFlag<ComponentsT...>(flagMask);

	collection_.ForEach([this, flagMask, &_func](const size_t _index, IObject& _object, ComponentFlag _flag)
	{
		if ((_flag & flagMask) == flagMask)
		{
			_func(_index, _object, collection_.template GetComponent<ComponentsT>(_object.GetId())...);
		}
	});
}

template<typename ...ComponentsT>
template<typename Func>
inline void cha::View<ComponentsT...>::AtFast(const size_t _index, Func&& _func)
{
	collection_.AtFast(_index, [this, &_func](IObject& _object)
	{
		_func(_object, collection_.template GetComponent<ComponentsT>(_object.GetId())...);
	});
}

template<typename ...ComponentsT>
template<typename FirstT, typename ...Rest>
inline void cha::View<ComponentsT...>::GetComponentFlag(ComponentFlag& _flag)
{
	const uint64_t INDEX = ComponentRegistry::GetComponentIndex<FirstT>();
	_flag[INDEX] = true;

	if constexpr (sizeof...(Rest) > 0)
	{
		GetComponentFlag<Rest...>(_flag);
	}
	else
	{
		GetComponentFlag(_flag);
	}
}
