#pragma once
#include "../../BasePch.h"
#include "IComponentPool.h"

namespace cha
{
	/// @brief コンポーネントのポインタプール
	/// @tparam ComponentT 対象のコンポーネント型
	/// @todo ゲームオブジェクトにつくのではなくプールが所有していたほうがキャッシュ効率いい…
	template<typename ComponentT>
	class ComponentPool : public IComponentPool
	{
	public:
		/// @brief コンポーネントのポインタプールの初期化処理
		ComponentPool() = default;
		/// @brief コンポーネントのポインタプールの解放処理
		~ComponentPool() = default;

		/// @brief 他コンポーネントプールと同調するためにサイズを指定する
		/// @param _size 指定のサイズ
		inline void PaddingSize(const size_t _size) override
		{
			// 指定のサイズになるまで nullptrで埋める
			while (pool_.size() <= _size)
			{
				pool_.push_back(nullptr);
			}
		}

		/// @brief 指定コンポーネントを参照する
		/// @param _registryIndex レジストリインデクス
		/// @return コンポーネントの参照ポインタ
		inline ComponentT& At(const size_t _registryIndex)
		{
			// 指定コンポーネントまで nullptrで埋める
			while (pool_.size() <= _registryIndex)
			{
				pool_.push_back(nullptr);
			}

			VCU_ASSERT(pool_[_registryIndex]
				&& "指定コンポーネントのポインタを持っていない");
			return *pool_[_registryIndex];
		}

		/// @brief 指定コンポーネントを末端とスワップする
		/// @param _registryIndex レジストリインデクス
		inline void SwapToBack(const size_t _registryIndex) override
		{
			std::swap(pool_[_registryIndex], pool_.back());
		}
		
		/// @brief 末端のコンポーネントをポイ捨てする
		inline void PopBack() override { pool_.pop_back(); }

		/// @brief コンポーネントをセットする
		/// @param _registryIndex レジストリインデクス
		/// @param _pComponent コンポーネントのポインタ
		inline void Set(const size_t _registryIndex, ComponentT* _pComponent)
		{
			while (pool_.size() <= _registryIndex)
			{
				pool_.push_back(nullptr);
			}
			pool_[_registryIndex] = _pComponent;
		}

	private:
		std::vector<ComponentT*> pool_{};  ///< コンポーネントのポインタプール
	};
}
