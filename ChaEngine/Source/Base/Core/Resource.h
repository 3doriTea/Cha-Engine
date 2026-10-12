#pragma once
#include "../BasePch.h"
#include "ResourceTraits.h"

namespace cha
{
	template<ResourceType ResourceTypeV>
	class Resource
	{
		// Traitsから対応する型を取得
		using DataT = ResourceData_t<ResourceTypeV>;
		using RegistryT = ResourceRegistry_t<ResourceTypeV>;
		using RegistryLoadConfigT = ResourceRegistryLoadConfig_t<ResourceTypeV>;

	public:
		inline Resource();
		inline ~Resource() = default;

		// リソースはコピーもムーブも許可
		inline Resource(const Resource&) = default;
		inline Resource(Resource&&) = default;
		inline Resource& operator=(const Resource&) = default;
		inline Resource& operator=(Resource&&) = default;

		/// <summary>
		/// データロードの設定を指定して読み込む
		/// </summary>
		/// <typeparam name="...Args">データロードの設定構造体の引数</typeparam>
		/// <param name="..._args">データロードの設定構造体の引数</param>
		template<typename ...Args>
			//requires std::constructible_from<RegistryLoadConfigT, Args...>
		inline Resource(Args&&... _args);

		/// @brief リソースを直接入れる
		/// @param _data データの右辺値
		inline Resource(DataT&& _data);

		/// @brief リソースを取得
		/// @return リソースの参照ポインタ
		inline DataT& Get();

		/// @brief リソースを取得
		/// @return リソースの参照ポインタ
		inline DataT& Get() const;

		/// @brief リソースが未指定か
		/// @return 未指定 true / false
		inline bool IsNull() const { return handle_ == INVALID_HANDLE; }
		
		/// @brief リソースを直接bool型に変換
		inline operator bool() const { return !IsNull(); }

	private:
		Handle handle_{ INVALID_HANDLE };  // リソースのハンドル

	private:
		static Handle handleCounter_;  // ハンドルのカウンタ
	};
}

#include "Resource.inl"
