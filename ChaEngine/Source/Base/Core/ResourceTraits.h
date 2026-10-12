#pragma once
#include "Resource/ResourceType.h"

namespace cha
{
	// デフォルト定義（未定義のタイプ用）
	template<ResourceType T> struct ResourceTraits;

	// 短縮用のエイリアス
	template<ResourceType T> using ResourceData_t = typename ResourceTraits<T>::DataT;
	template<ResourceType T> using ResourceRegistry_t = typename ResourceTraits<T>::RegistryT;
	template<ResourceType T> using ResourceRegistryLoadConfig_t = typename ResourceTraits<T>::LoadConfigT;
}
