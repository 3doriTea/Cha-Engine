
#include "SceneBase.h"
#include "GameObjectCollection.h"

cha::SceneBase::SceneBase() :
	pGameObjectCollection_{ std::make_unique<GameObjectCollection>() }
{
}

void cha::SceneBase::Destroy(GameObject _gameObject)
{
	pGameObjectCollection_->DestroyAt(_gameObject);
}

void cha::SceneBase::Update()
{
	pGameObjectCollection_->ForEach([](IObject& _object)
	{
		_object.Update();
	});
}

void cha::SceneBase::LateUpdate()
{
	pGameObjectCollection_->ForEachSystem([](ISystemObject& _systemObject)
	{
		_systemObject.LateUpdate();
	});
}

void cha::SceneBase::Draw(const bool _isShadowing)
{
	if (_isShadowing)
	{
		pGameObjectCollection_->ForEach([](IObject& _object)
		{
			_object.DrawShadowMap();
		});
	}
	else
	{
		pGameObjectCollection_->ForEach([](IObject& _object)
		{
			_object.Draw();
		});
	}
}

void cha::SceneBase::End()
{
	pGameObjectCollection_->ForEach([](IObject& _object)
	{
		_object.OnDestroy();
	});
	pGameObjectCollection_->Clear();
}

cha::GameObjectCollection& cha::SceneBase::RefGameObjectCollection()
{
	return *pGameObjectCollection_.get();
}
