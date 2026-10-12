#include "ComponentPools.h"
#include "../ComponentBase.h"

struct NullComponent
{
};

cha::ComponentPools::ComponentPools()
{
	// NullComponentを一番最後に登録することで+1=全コンポーネント数が求まる
	const size_t INDEX{ ComponentRegistry::GetComponentIndex<NullComponent>() };
	pool_.resize(INDEX + 1);
}

void cha::ComponentPools::PaddingSize(const size_t _size)
{
	for (auto& spIPool : pool_)
	{
		if (spIPool)
		{
			spIPool->PaddingSize(_size);
		}
	}
}

void cha::ComponentPools::EraseAt(const size_t _registryIndex)
{
	// もしレジストリの末端を消す場合はスワップせずに削除できる
	if (_registryIndex == pool_.size() - 1)
	{
		// 全コンポーネントプールを周回
		for (auto& spIPool : pool_)
		{
			if (spIPool)  // かつインスタンスがある
			{
				spIPool->PopBack();
			}
		}
	}
	else
	{
		// 全コンポーネントプールを周回
		for (auto& spIPool : pool_)
		{
			if (spIPool)  // かつインスタンスがある
			{
				// 指定レジストリのインデクスと末端を入れ替え
				spIPool->SwapToBack(_registryIndex);
				// 入れ替えた末端をポイ捨てする
				spIPool->PopBack();
			}
		}
	}
}
