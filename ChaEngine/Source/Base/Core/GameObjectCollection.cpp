#include "GameObjectCollection.h"
#include "NullObjectSystem.h"
#include "SystemRegistry.h"
#include "ComponentPool/ComponentPools.h"


cha::GameObjectCollection::GameObjectCollection() :
	// NOTE: 外システムの登録が終わったコンストラクタタイミングで取得
	//     : index == システム数 - 1
	//     : NullObjectSystemを含め +1 で総数
	SYSTEM_COUNT_{ SystemRegistry::GetSystemIndex<NullObjectSystem>() + 1 },
	pComponentPools_{ std::make_unique<ComponentPools>() }
{
	// 各システム分開けておく
	for (size_t i = 0; i < SYSTEM_COUNT_; i++)
	{
		indices_.push_back(i);
	}
	// レジストリもシステムの分だけ空けておく
	registry_.resize(SYSTEM_COUNT_);

	// Id 0 は NullObjectSystem を登録しておく
	const size_t NULL_OBJECT_SYSTEM_INDEX{ 0 };
	
	GameObjectElement& element{ registry_[NULL_OBJECT_SYSTEM_INDEX] };
	element.pObject = std::make_unique<NullObjectSystem>();
	element.typeIndex = typeid(NullObjectSystem);
	element.componentFlag = 0;

	registry_.resize(SYSTEM_COUNT_);
	systemsSubRegistry_.resize(SYSTEM_COUNT_);
}

cha::GameObjectCollection::~GameObjectCollection()
{
}

void cha::GameObjectCollection::Flush()
{
	// 削除予定のオブジェクトを削除してく
	for (const uint64_t ID : toDestroyIds_)
	{
		const size_t REGISTRY_INDEX = IdToRegistryIndex(ID);
		registry_[REGISTRY_INDEX].pObject->OnDestroy();
	}
	for (const uint64_t ID : toDestroyIds_)
	{
		Remove(ID);
	}
	toDestroyIds_.clear();

	// Start呼び出し中にInstantiateした場合に対応し1フレームでStartしきる
	while (!toAdd_.empty())
	{
		std::vector<GameObjectElement> toAdd = std::move(toAdd_);

		// 追加前のレジストリサイズを持っておく
		const size_t PREVIOUS_REGISTRY_SIZE{ registry_.size() };

		// 登録予定のオブジェクトを登録していく
		for (size_t i = 0; i < toAdd.size(); i++)
		{
			// ここで改めてindicesを修正
			const uint64_t ID = toAdd[i].pObject->GetId();

			// レジストリインデクスを登録する
			indices_[ID] = registry_.size();
			registry_.emplace_back(std::move(toAdd[i]));

			// コンポーネントの登録もここでする
			UpdateComponentRegistry(registry_.back().componentFlag, ID, i);
		}
		// 追加した分の全体調整
		FillComponentRegistry();

		// 登録したら一気に開始処理
		for (size_t i = PREVIOUS_REGISTRY_SIZE; i < registry_.size(); i++)
		{
			registry_[i].pObject->Start();
		}
	}
}

cha::GameObject cha::GameObjectCollection::MakeGameObject(const uint64_t _id)
{
	return GameObject{ _id, this };
}

void cha::GameObjectCollection::Remove(const uint64_t _id)
#if 0
{
	//const size_t INDEX{ indices_[_id] };

	//VCU_ASSERT(INDEX != 0);

	//indices_[_id] = 0;  // 無効INDEXにする

	//// 末尾のインデクスを変更
	//indices_.back() = INDEX;

	//// 削除要素を末尾に持ってくる
	//std::swap(registry_[INDEX], registry_.back());
	//std::swap(componentFlags_[INDEX], componentFlags_.back());

	//// 末尾を削除する
	//registry_.pop_back();
	//componentFlags_.pop_back();

	const size_t INDEX{ indices_[_id] };

	VCU_ASSERT(INDEX != 0);

	// もし削除対象が「たまたま現在の末尾要素」だった場合は、Swapせずにそのまま消すだけでよい
	const size_t LAST_INDEX = registry_.size() - 1;
	if (INDEX == LAST_INDEX)
	{
		indices_[_id] = 0;
		registry_.pop_back();
		componentFlags_.pop_back();
		return;
	}

	// --- ここから要素が入れ替わる場合の処理 ---

	// 1. 引っ越しすることになる「現在の末尾オブジェクト」のIDを取得する
	// ※ IObjectに GetId() のようなIDを返す関数が定義されている前提です
	const uint64_t LAST_OBJECT_ID = registry_.back()->GetId();

	// 2. 引っ越し先のインデックス（INDEX）を、そのオブジェクトの逆引きとして登録し直す
	indices_[LAST_OBJECT_ID] = INDEX;

	// 3. 自分自身のIDは無効(0)にする
	indices_[_id] = 0;

	// 4. 削除要素を末尾に持ってきて pop_back
	std::swap(registry_[INDEX], registry_.back());
	std::swap(componentFlags_[INDEX], componentFlags_.back());

	registry_.pop_back();
	componentFlags_.pop_back();
}
#else
{
	const size_t INDEX{ IdToRegistryIndex(_id) };

	VCU_ASSERT(INDEX != 0
		&& "NullObjectSystemは削除しないでください！！");

	// もし削除対象がたまたま現在の末尾要素だった場合は、Swapせずにそのまま消すだけでよい
	const size_t LAST_INDEX = registry_.size() - 1;
	if (INDEX == LAST_INDEX)
	{
		// コンポーネントプールのほうを先に消す
		pComponentPools_->EraseAt(INDEX);
		indices_[_id] = 0;
		registry_.pop_back();
		return;
	}

	const uint64_t LAST_OBJECT_ID = registry_.back().pObject->GetId();
	indices_[LAST_OBJECT_ID] = INDEX;  // 末端は対象の位置へ
	indices_[_id] = 0;  // 捨てるものは無効化

	// レジストリ内入れ替え
	std::swap(registry_[INDEX], registry_.back());

	// コンポーネントを消す
	pComponentPools_->EraseAt(INDEX);

	// 入れ替えた末端を捨てる
	registry_.pop_back();
}
#endif

size_t cha::GameObjectCollection::IdToRegistryIndex(const uint64_t _id) const
{
	return indices_[_id];
}

bool cha::GameObjectCollection::IsRegisteredIndex(const size_t _index) const
{
	return _index < registry_.size();
}

bool cha::GameObjectCollection::IsPendingIndex(const size_t _index) const
{
	return registry_.size() <= _index && _index <= (registry_.size() + toAdd_.size());
}

void cha::GameObjectCollection::FillComponentRegistry()
{
	// 追加し終えたレジストリはクリアする
	PreviousComponentRegister::registry_.clear();

	// 各コンポーネントのプールサイズを統一する
	pComponentPools_->PaddingSize(registry_.size());
}

void cha::GameObjectCollection::UpdateComponentRegistry(const ComponentFlag _flag, const uint64_t _gameObjectId, const size_t _addedIndex)
{
	// TODO: キャパシティ分線形探索は非効率？
	for (uint32_t componentIndex = 0; componentIndex < COMPONENT_CAPACITY; componentIndex++)
	{
		if (_flag.test(componentIndex))
		{
			// 指定コンポーネントのフラグが立っているなら登録関数が更新されている
			PreviousComponentRegister::registry_[_addedIndex][componentIndex](
				*pComponentPools_.get(),
				IdToRegistryIndex(_gameObjectId));
		}
	}
}

void cha::GameObjectCollection::DestroyAt(const GameObject _gameObject)
{
	// 削除予定リストに入れるだけ
	toDestroyIds_.insert(_gameObject.id_);
}

void cha::GameObjectCollection::Clear()
{
	indices_.clear();
	registry_.clear();
	toAdd_.clear();
	toDestroyIds_.clear();
	pComponentPools_ = std::make_unique<ComponentPools>();
}

bool cha::GameObjectCollection::Exist(const GameObject _gameObject) const
{
	return Exist(_gameObject.GetId());
}

bool cha::GameObjectCollection::Exist(const uint64_t _id) const
{
	const size_t REGISTRY_INDEX = IdToRegistryIndex(_id);
	return REGISTRY_INDEX != 0                 // NullObjectSystemを参照しているわけではない！
		&& REGISTRY_INDEX < registry_.size()   // レジストリのサイズ内
		&& registry_[REGISTRY_INDEX].pObject;  // オブジェクトのポインタが存在する
}
