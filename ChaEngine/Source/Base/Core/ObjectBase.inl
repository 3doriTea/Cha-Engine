#include "ObjectBase.h"
#include "IComponent.h"
#include <Base/Manager/SceneManager.h>
#include "../ManagerRegistry.h"
#include "../Core/SceneBase.h"

namespace cha
{
	template<typename DataT>
	inline ObjectBase<DataT>::ObjectBase(DataT&& _data) :
		DataT{ std::move(_data) }
	{
	}

	template<typename DataT>
	inline ObjectBase<DataT>::ObjectBase(const DataT& _data) :
		DataT{ _data }
	{
	}

	template<typename DataT>
	inline GameObject ObjectBase<DataT>::ToGameObject() const
	{
		return ManagerRegistry::Ref<SceneManager>().MakeGameObject(id_);
	}

	template<typename DataT>
	inline void ObjectBase<DataT>::DestroyMe()
	{
		// シーンマネージャーに自分自身のゲームオブジェクトを消してもらう
		ManagerRegistry::Ref<SceneManager>().DestroyAt(ToGameObject());
	}

	template<typename DataT>
	template<typename ...Args>
	inline ObjectBase<DataT>::ObjectBase(Args&& ..._args) requires std::constructible_from<DataT, Args...> :
		DataT{ std::forward<Args>(_args)... }
	{
	}

	template<typename DataT>
	template<std::derived_from<ISystemObject> T>
	inline T& ObjectBase<DataT>::Get()
	{
		return ManagerRegistry::Ref<SceneManager>().template GetSystemObject<T>();
	}

	template<typename DataT>
	template<std::derived_from<IManager> T>
	inline T& ObjectBase<DataT>::Get()
	{
		return ManagerRegistry::Ref<T>();
	}
	
	template<typename DataT>
	template<typename ...ComponentsT>
	inline View<ComponentsT...> ObjectBase<DataT>::GetView()
	{
		return ManagerRegistry::Ref<SceneManager>().template GetView<ComponentsT...>();
	}

	template<typename DataT>
	template<typename SceneT>
	inline SceneT& ObjectBase<DataT>::GetScene() const
	{
		return ManagerRegistry::Ref<SceneManager>().GetCurrentScene();
	}

	template<typename DataT>
	template<std::derived_from<IComponent> T>
	inline T& ObjectBase<DataT>::Get(const GameObject _gameObject)
	{
		VCU_ASSERT(!_gameObject.IsEmpty() && "ゲームオブジェクトが空っぽ");
		return *ManagerRegistry::Ref<SceneManager>().template FindComponent<T>(_gameObject);
	}

	template<typename DataT>
	template<std::derived_from<IComponent> T>
	inline T* ObjectBase<DataT>::TryGet(const GameObject _gameObject)
	{
		VCU_ASSERT(!_gameObject.IsEmpty() && "ゲームオブジェクトが空っぽ");
		return ManagerRegistry::Ref<SceneManager>().template FindComponent<T>(_gameObject);
	}
	
	template<typename DataT>
	template<typename T>
	inline T& ObjectBase<DataT>::Get(const GameObject _gameObject)
	{
		return ManagerRegistry::Ref<SceneManager>().template FindObject<T>(_gameObject);
	}
}
