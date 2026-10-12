#pragma once
#include "../BasePch.h"
#include "../CoreType/FixedString.h"
#include "../Core/Resource/ResourceType.h"
#include "Resource/IResourceRegistry.h"
#include "Resource/ResourceRegistryBase.h"

namespace cha
{
	/// @brief リソース管理 シングルトンクラス リソースは実行環境で一元管理のため。
	template<FixedString RESOURCE_DIR_PATH>
	class ResourceManager final
	{
		template<ResourceType ResourceTypeValue>
		friend class Resource;
		friend class GameLoop;
	public:
		// コピー禁止
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;

		// ムーブ禁止
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

	private:
		/// @brief シングルトンパターンのインスタンスを取得
		/// @return インスタンスの参照ポインタ
		static ResourceManager& Instance()
		{
			static ResourceManager instance{};
			return instance;
		}

		inline ResourceManager()
		{
			fs::current_path(RESOURCE_DIR_PATH.buffer);
		}
		~ResourceManager() = default;

		/// @brief リソースのハンドルを追加 or 取得する
		/// @tparam ResourceRegistryT レジストリ型
		/// @tparam ResourceTypeV レジストリのENUM
		/// @param _file ファイルパス
		/// @return ハンドル
		#if 0
		template<ResourceType ResourceTypeV, typename ResourceRegistryT>
		inline Handle AddOrGet(const fs::path& _file)
		{
			if (resources_.count(ResourceTypeV) == 0)
			{
				resources_[ResourceTypeV] = std::make_shared<ResourceRegistryT>();
			}
			return std::static_pointer_cast<ResourceRegistryT>(resources_[ResourceTypeV])
				->AddOrGet(_file);
		}
		#endif

		template<ResourceType ResourceTypeV, typename ResourceRegistryT, typename ...Args>
		inline Handle AddOrGet(Args&&... _args)
		{
			if (resources_.count(ResourceTypeV) == 0)
			{
				resources_[ResourceTypeV] = std::make_shared<ResourceRegistryT>();
			}

			// レジストリが求めている Config の型（TextureLoadConfig など）を取得
			using ConfigT = typename ResourceRegistryT::LoadConfigT;

			return std::static_pointer_cast<ResourceRegistryT>(resources_[ResourceTypeV])
				->AddOrGet(ConfigT{ std::forward<Args>(_args)... });
		}

		template<ResourceType ResourceTypeV, typename ResourceRegistryT>
		inline Handle Add(ResourceRegistryT::DataT&& _data)
		{
			if (resources_.count(ResourceTypeV) == 0)
			{
				resources_[ResourceTypeV] = std::make_shared<ResourceRegistryT>();
			}
			return std::static_pointer_cast<ResourceRegistryT>(resources_[ResourceTypeV])
				->Add(std::move(_data));
		}

		/// @brief リソースを取得する
		/// @tparam DataT リソースデータ構造体型
		/// @tparam ResourceRegistryT レジストリ型
		/// @tparam ResourceTypeV レジストリのENUM
		/// @param _handle リソースハンドル
		/// @return リソースデータの参照ポインタ
		template<ResourceType ResourceTypeV, typename ResourceRegistryT>
		inline ResourceRegistryT::DataT& GetResource(const Handle _handle)
		{
			if (resources_.count(ResourceTypeV) == 0)
			{
				resources_[ResourceTypeV] = std::make_shared<ResourceRegistryT>();
			}
			//auto spResourceRegistryBase = //std::static_pointer_cast<ResourceRegistryBase<DataT>>(resources_[ResourceTypeV]);
			auto spResourceRegistry = std::static_pointer_cast<ResourceRegistryT>(resources_[ResourceTypeV]);
			return spResourceRegistry->GetResource(_handle);
		}

		template<ResourceType ResourceTypeV, typename ResourceRegistryT>
		inline ResourceRegistryT::DataT& GetResource(const Handle _handle) const
		{
			VCU_ASSERT(resources_.count(ResourceTypeV) != 0 && "リソースは未登録");
			auto spResourceRegistry = std::static_pointer_cast<ResourceRegistryT>(resources_[ResourceTypeV]);
			return spResourceRegistry->GetResource(_handle);
		}

		/// @brief リソースをすべて解放する
		inline void Clear()
		{
			resources_.clear();
		}

	private:
		// 各リソースのレジストリ
		std::map<ResourceType, std::shared_ptr<IResourceRegistry>> resources_{};
	};
}
