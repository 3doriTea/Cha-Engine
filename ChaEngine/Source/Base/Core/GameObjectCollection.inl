#include "GameObjectCollection.h"
#include "ComponentPool/ComponentPools.h"
#include "SystemRegistry.h"
#include "PreviousComponentFlag.h"
#include "PreviousComponentRegister.h"

namespace cha
{
	// システムオブジェクトの追加
	template<std::derived_from<ISystemObject> SystemObjectT, typename ...Args>
	inline GameObject GameObjectCollection::Add(Args&&... _args)
	{
		// NOTE: SystemObjectの場合はindexがそのままGameObjectIdになる
		const uint64_t INDEX{ SystemRegistry::GetSystemIndex<SystemObjectT>() };

		PreviousComponentFlag::flag_.reset();
		// コンポーネント登録用レジストリに枠を追加
		PreviousComponentRegister::registry_.push_back({});

		auto p = std::make_shared<SystemObjectT>(_args...);
		
		// コンポーネントフラグをコピーしてリセット
		ComponentFlag componentFlag = PreviousComponentFlag::flag_;
		PreviousComponentFlag::flag_.reset();

		// コンポーネントレジストリの更新
		UpdateComponentRegistry(componentFlag, INDEX, 0);

		// その場で追加したため埋める処理
		FillComponentRegistry();

		// コンポーネントフラグをセット
		registry_[INDEX].componentFlag = componentFlag;

		// 割り当てられたオブジェクトのIdをセットする
		p->SetId(INDEX);
		registry_[INDEX].pObject = std::static_pointer_cast<IObject>(p);
		systemsSubRegistry_[INDEX] = std::static_pointer_cast<ISystemObject>(p);
		
		// 型情報も追加する
		registry_[INDEX].typeIndex = std::type_index{ typeid(SystemObjectT) };
		
		// システムオブジェクトは特別生成時に直接開始処理を呼び出す
		registry_[INDEX].pObject->Start();

		return GameObject{ INDEX, this };
	}

	template<typename ObjectT, typename ...Args>
	inline GameObject GameObjectCollection::Add(Args&& ..._args)
	{
		// Idは一意に割り当てるためにindicesは追加する
		const uint64_t ID{ indices_.size() };
		// この場で追加してしまうとまずいかもAA
		indices_.push_back(registry_.size() + toAdd_.size() - toDestroyIds_.size());

		PreviousComponentFlag::flag_.reset();
		// コンポーネント登録用レジストリに枠を追加
		PreviousComponentRegister::registry_.push_back({});

		auto p = std::make_shared<ObjectT>(std::forward<Args>(_args)...);

		// コンポーネントフラグをコピーしてリセット
		ComponentFlag componentFlag = PreviousComponentFlag::flag_;
		PreviousComponentFlag::flag_.reset();

		// NOTE: コンポーネントレジストリの更新はここでしない
		//UpdateComponentRegistry(componentFlag, ID);

		// 割り当てられたオブジェクトIdを適用する
		p->SetId(ID);

		// 次のループの予定に追加する
		toAdd_.emplace_back(
			std::static_pointer_cast<IObject>(p),
			std::type_index{ typeid(ObjectT) },
			componentFlag);

		return GameObject{ ID, this };
	}

	template<typename ObjectT>
	inline ObjectT& GameObjectCollection::At(const GameObject _gameObject)
	{
		return At<ObjectT>(_gameObject.GetId());
	}
	
	template<typename ObjectT>
	inline ObjectT& GameObjectCollection::At(const uint64_t _id)
	{
		std::shared_ptr<ObjectT> spObject = nullptr;

		const size_t INDEX{ IdToRegistryIndex(_id) };

		GameObjectElement* pElement{ nullptr };
		if (IsRegisteredIndex(INDEX))
		{
			// レジストリ内から要素を見つける
			pElement = &registry_[INDEX];
		}
		else if (IsPendingIndex(INDEX))
		{
			// 追加予定の列から要素を見つける
			pElement = &toAdd_[INDEX - registry_.size()];
		}

		VCU_ASSERT(pElement && "指定ゲームオブジェクトの型が一致しないか、インスタンスされていません。");

		// IObjectでstatic_pointer_castできるか判定
		if constexpr (std::is_base_of_v<IObject, ObjectT> || std::is_base_of_v<ObjectT, IObject>)
		{
			spObject = std::static_pointer_cast<ObjectT>(pElement->pObject);
		}
		else
		{
			spObject = std::dynamic_pointer_cast<ObjectT>(pElement->pObject);
		}

		VCU_ASSERT(spObject && "指定ゲームオブジェクトの型が一致しないか、インスタンスされていません。");

		return *spObject;
	}

	template<typename ComponentT>
	inline ComponentT* GameObjectCollection::FindComponent(const GameObject _gameObject)
	{
		return FindComponent<ComponentT>(_gameObject.GetId());
	}

	template<typename ComponentT>
	inline ComponentT* GameObjectCollection::FindComponent(const uint64_t _id)
	{
		const size_t INDEX{ IdToRegistryIndex(_id) };
		const uint64_t COMPONENT_INDEX{ ComponentRegistry::GetComponentIndex<ComponentT>() };

		if (IsRegisteredIndex(INDEX))
		{
			if (!registry_[INDEX].componentFlag.test(COMPONENT_INDEX))
			{
				return nullptr;  // 指定コンポーネントが含まれていないためnullptrを返す
			}
			return &pComponentPools_->template At<ComponentT>(INDEX);
		}
		else if (IsPendingIndex(INDEX))
		{
			return nullptr;
			//const size_t PENDING_INDEX = INDEX - registry_.size();

			//if (!toAdd_[PENDING_INDEX].componentFlag.test(COMPONENT_INDEX))
			//{
			//	return nullptr;  // 指定コンポーネントが含まれていないためnullptrを返す
			//}
			//return &pComponentPools_->template At<ComponentT>(INDEX);
		}
		else
		{
			// 単純に範囲外なら nullptr を返す
			return nullptr;
		}
	}

	template<typename Func>
	inline void GameObjectCollection::AtFast(const uint64_t _index, Func&& _fanc)
	{
		// 直接 index アクセスしていく

		if constexpr (std::is_invocable_v<Func, IObject&, ComponentFlag>)
		{
			auto& spObject = registry_[_index].pObject;
			if (spObject)
			{
				_fanc(*spObject.get(), registry_[_index].componentFlag);
			}
		}
		else if constexpr (std::is_invocable_v<Func, ComponentFlag, IObject&>)
		{
			auto& spObject = registry_[_index].pObject;
			if (spObject)
			{
				_fanc(registry_[_index].componentFlag, *spObject.get());
			}
		}
		else if constexpr (std::is_invocable_v<Func, IObject&>)
		{
			auto& spObject = registry_[_index].pObject;
			if (spObject)
			{
				_fanc(*spObject.get());
			}
		}
		else if constexpr (std::is_invocable_v<Func, ComponentFlag>)
		{
			
			auto& spObject = registry_[_index].pObject;
			if (spObject)
			{
				_fanc(registry_[_index].componentFlag);
			}
		}
		else
		{
			static_assert(
				dependent_false<Func>,
				"IObject&かComponentFlagまたはその両方を受け取る引数を用意してください。");
		}
	}

	template<typename Func>
	inline void GameObjectCollection::ForEach(Func&& _callback)
	{
		if constexpr (std::is_invocable_v<Func, IObject&, ComponentFlag>)
		{
			for (auto& [spObject, typeIndex, componentFlag] : registry_)
			{
				if (spObject && spObject->IsActive())
				{
					_callback(*spObject.get(), componentFlag);
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, size_t, IObject&, ComponentFlag>)
		{
			for (size_t i{ 1 }; i < registry_.size(); i++)
			{
				auto& spObject = registry_[i].pObject;
				if (spObject && spObject->IsActive())
				{
					_callback(i, *spObject.get(), registry_[i].componentFlag);
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, ComponentFlag, IObject&>)
		{
			for (auto& [spObject, typeIndex, componentFlag] : registry_)
			{
				if (spObject && spObject->IsActive())
				{
					_callback(componentFlag, *spObject.get());
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, IObject&>)
		{
			for (auto& [spObject, typeIndex, componentFlag] : registry_)
			{
				if (spObject && spObject->IsActive())
				{
					_callback(*spObject.get());
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, ComponentFlag>)
		{
			for (auto& [spObject, typeIndex, componentFlag] : registry_)
			{
				if (spObject && spObject->IsActive())
				{
					_callback(componentFlag);
				}
			}
		}
		else
		{
			static_assert(
				dependent_false<Func>,
				"IObject&かComponentFlagまたはその両方を受け取る引数を用意してください。");
		}
	}

	template<typename Func>
	inline void GameObjectCollection::ForEachSystem(Func&& _callback)
	{
		if constexpr (std::is_invocable_v<Func, ISystemObject&, ComponentFlag>)
		{
			// システムオブジェクトの範囲で周回する
			for (size_t i{ 1 }; i < SYSTEM_COUNT_; i++)
			{
				auto& spObject = registry_[i].pObject;
				if (spObject && spObject->IsActive())
				{
					_callback(*systemsSubRegistry_[i].get(), registry_[i].componentFlag);
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, size_t, ISystemObject&, ComponentFlag>)
		{
			// システムオブジェクトの範囲で周回する
			for (size_t i{ 1 }; i < SYSTEM_COUNT_; i++)
			{
				auto& spObject = registry_[i].pObject;
				if (spObject && spObject->IsActive())
				{
					_callback(i, *systemsSubRegistry_[i].get(), registry_[i].componentFlag);
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, ComponentFlag, ISystemObject&>)
		{
			// システムオブジェクトの範囲で周回する
			for (size_t i{ 1 }; i < SYSTEM_COUNT_; i++)
			{
				auto& spObject = registry_[i].pObject;
				if (spObject && spObject->IsActive())
				{
					_callback(registry_[i].componentFlag, *systemsSubRegistry_[i].get());
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, ISystemObject&>)
		{
			// システムオブジェクトの範囲で周回する
			for (size_t i{ 1 }; i < SYSTEM_COUNT_; i++)
			{
				auto& spObject = registry_[i].pObject;
				if (spObject && spObject->IsActive())
				{
					_callback(*systemsSubRegistry_[i].get());
				}
			}
		}
		else if constexpr (std::is_invocable_v<Func, ComponentFlag>)
		{
			// システムオブジェクトの範囲で周回する
			for (size_t i{ 1 }; i < SYSTEM_COUNT_; i++)
			{
				auto& spObject = registry_[i].pObject;
				if (spObject && spObject->IsActive())
				{
					_callback(registry_[i].componentFlag);
				}
			}
		}
		else
		{
			static_assert(
				dependent_false<Func>,
				"ISystemObject&かComponentFlagまたはその両方を受け取る引数を用意してください。");
		}
	}

	template<typename ...ComponentsT>
	inline auto GameObjectCollection::GetView()
	{
		return View<ComponentsT...>{ *this };
	}
	
	template<typename ComponentT>
	inline ComponentT& GameObjectCollection::GetComponent(const uint64_t _id)
	{
		const size_t INDEX{ IdToRegistryIndex(_id) };
		const uint64_t COMPONENT_INDEX{ ComponentRegistry::GetComponentIndex<ComponentT>() };

		VCU_ASSERT(registry_[INDEX].componentFlag.test(COMPONENT_INDEX)
			&& "指定コンポーネントが含まれていない");

		return pComponentPools_->template At<ComponentT>(INDEX);
	}
}
