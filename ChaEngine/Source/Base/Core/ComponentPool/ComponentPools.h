#pragma once
#include "../../BasePch.h"
#include "IComponentPool.h"


namespace cha
{
	/// @brief コンポーネントプールを管理する
	class ComponentPools
	{
	public:
		/// @brief コンポーネントプールを初期化処理
		ComponentPools();
		/// @brief コンポーネントプールの解放処理
		~ComponentPools() = default;

		/// @brief コンポーネントのポインタをプールに登録する
		/// @tparam ComponentT 
		/// @param _registryIndex レジストリのインデクス
		/// @param _pComponent 
		template<typename ComponentT>
		void Register(const size_t _registryIndex, ComponentT* _pComponent);

		/// @brief 各コンポーネントのプールサイズを指定する
		/// @param _size 指定サイズ
		void PaddingSize(const size_t _size);

		/// @brief コンポーネントを参照する
		/// @tparam ComponentT コンポーネントの型
		/// @param _registryIndex レジストリのインデクス
		/// @return コンポーネントの参照ポインタ
		template<typename ComponentT>
		ComponentT& At(const size_t _registryIndex);

		/// @brief 指定のコンポーネントを削除する
		/// @param _registryIndex レジストリのインデクス
		void EraseAt(const size_t _registryIndex);

	private:
		std::vector<std::shared_ptr<IComponentPool>> pool_{};  ///< 各コンポーネント分のプール配列 要素数はコンポーネントの種類数
	};
}

#include "ComponentPools.inl"
