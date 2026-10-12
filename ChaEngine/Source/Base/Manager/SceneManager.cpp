
#include "SceneManager.h"
#include "../Core/IScene.h"
#include "../Core/SceneBase.h"


cha::SceneManager::SceneManager()
{
}

cha::SceneManager::~SceneManager()
{
}

cha::SceneBase& cha::SceneManager::GetCurrentScene()
{
	return *static_cast<SceneBase*>(pCurrentScene_.get());
}

cha::GameObject cha::SceneManager::MakeGameObject(const uint64_t _id)
{
	return pCurrentScene_->RefGameObjectCollection().MakeGameObject(_id);
}

void cha::SceneManager::DestroyAt(const GameObject _gameObject)
{
	pCurrentScene_->RefGameObjectCollection().DestroyAt(_gameObject);
}

bool cha::SceneManager::Update()
{
	if (!pCurrentScene_)
	{
		return false;
	}
	else
	{
		pCurrentScene_->Update();
	}
	return true;
}

bool cha::SceneManager::LateUpdate()
{
	if (!pCurrentScene_)
	{
		return false;
	}
	else
	{
		pCurrentScene_->LateUpdate();
	}
	return true;
}

bool cha::SceneManager::Draw(const bool _isShadowing)
{
	if (!pCurrentScene_)
	{
		return false;
	}
	else
	{
		pCurrentScene_->Draw(_isShadowing);
	}
	return true;
}

bool cha::SceneManager::Refresh()
{
	// もし次のシーンがあるなら次のシーンへ遷移処理する
	if (pMakeNextSceneFactory_)
	{
		Clear();  // 現在のシーンはクリア
		(*pMakeNextSceneFactory_.get())();
		pMakeNextSceneFactory_.reset();
		pCurrentScene_->Start();
		return true;
	}

	// 予定オブジェクト系を処理
	pCurrentScene_->RefGameObjectCollection().Flush();

	return false;
}
