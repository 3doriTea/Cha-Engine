#include "Resource.h"
#include "../Core/ResourceManagerInstance.h"
#include "Resource/ResourceType.h"
#include "ResourceTraits.h"

namespace cha
{
	template<ResourceType ResourceTypeV>
	inline Resource<ResourceTypeV>::Resource()
	{
		// 対応表にある型を自動で使用
		/*handle_ = Resrc::Instance().Add<ResourceTypeV, RegistryT>({});
		VCU_ASSERT(handle_ != INVALID_HANDLE && "ハンドル読み込みに失敗")*/;
	}

	template<ResourceType ResourceTypeV>
	template<typename ...Args>
		//requires std::constructible_from<Resource<ResourceTypeV>::RegistryLoadConfigT, Args...>
	inline Resource<ResourceTypeV>::Resource(Args&& ..._args)
	{
		// 対応表にある型を自動で使用
		handle_ = Resrc::Instance().AddOrGet<ResourceTypeV, RegistryT>(std::forward<Args>(_args)...);
		VCU_ASSERT(handle_ != INVALID_HANDLE && "ハンドル読み込みに失敗");
	}

	template<ResourceType ResourceTypeV>
	inline cha::Resource<ResourceTypeV>::Resource(DataT&& _data)
	{
		handle_ = Resrc::Instance().Add<ResourceTypeV, RegistryT>(std::move(_data));
	}
	
	template<ResourceType ResourceTypeV>
	inline Resource<ResourceTypeV>::DataT& Resource<ResourceTypeV>::Get()
	{
		VCU_ASSERT(handle_ != INVALID_HANDLE && "無効ハンドルを取得しようとした");
		return Resrc::Instance().GetResource<ResourceTypeV, RegistryT>(handle_);
	}

	template<ResourceType ResourceTypeV>
	inline Resource<ResourceTypeV>::DataT& Resource<ResourceTypeV>::Get() const
	{
		VCU_ASSERT(handle_ != INVALID_HANDLE && "無効ハンドルを取得しようとした");
		return Resrc::Instance().GetResource<ResourceTypeV, RegistryT>(handle_);
	}
}
