#pragma once
#include "IMaterialProperty.h"

namespace cha
{
	/// @brief マテリアルプロパティ
	/// @tparam T マテリアルプロパティの型
	template<typename T>
	class MaterialProperty : public IMaterialProperty
	{
	public:
		MaterialProperty() = default;
		~MaterialProperty() = default;

		template<typename ...Args>
		MaterialProperty(Args&&... args)
			: property_{ std::forward<Args>(args)... }
		{
		}

		/// @brief プロパティ構造体のサイズを取得
		/// @return 構造体のサイズ
		inline UINT GetSize() override { return (static_cast<UINT>(sizeof(T)) + 15) & ~15; }
		
		/// @brief プロパティのポインタを取得
		/// @return ポインタ
		inline void* GetPtr() override { return &property_; }

		inline T& Get() { return property_; }
		
		T* operator->() { return &property_; }
		const T* operator->() const { return &property_; }

		T& operator*() { return property_; }
		const T& operator*() const { return property_; }

	private:
		alignas(16) T property_{};
	};
}


