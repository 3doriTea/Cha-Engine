#pragma once
#include "BasePch.h"
#include "Manager/IManager.h"

namespace cha
{
	/// @brief マネージャ登録
	class ManagerRegistry
	{
		friend class GameLoop;
	private:
		ManagerRegistry() = delete;
		~ManagerRegistry() = delete;

	public:
		/// @brief マネージャを参照
		/// @tparam T 指定マネージャ型
		/// @return マネージャの参照ポインタ
		template<typename T>
		inline static T& Ref()
		{
			const std::type_index TYPE_INDEX{ typeid(T) };

			VCU_ASSERT(registry_.count(TYPE_INDEX) && "未登録のマネージャが参照されました。");

			auto spT = std::static_pointer_cast<T>(registry_[TYPE_INDEX]);

			return *spT.get();
		}

	//private:
		/// @brief マネージャを登録する
		/// @tparam T 登録するマネージャ型
		/// @tparam ...Args 
		/// @param ..._args 
		template<typename T, typename ...Args>
		inline static T& Add(Args&&... _args)
		{
			const std::type_index TYPE_INDEX{ typeid(T) };
			// マネージャが未登録の場合登録する
			if (registry_.count(TYPE_INDEX) == 0)
			{
				std::shared_ptr<T> sp = std::make_shared<T>(_args...);
				registry_.emplace(TYPE_INDEX, std::static_pointer_cast<IManager>(sp));
				return *sp.get();
			}
			else
			{
				return Ref<T>();
			}
		}

		inline static void AllRelease()
		{
			for (auto& [type, pManager] : registry_)
			{
				pManager->Release();
			}
		}

	private:
		// 登録
		static std::map<std::type_index, std::shared_ptr<IManager>> registry_;
	};
}
