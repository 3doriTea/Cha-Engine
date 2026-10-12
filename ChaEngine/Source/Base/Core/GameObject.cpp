#include "GameObject.h"
#include "GameObjectCollection.h"

cha::GameObject::GameObject(const uint64_t _id , GameObjectCollection* _pGameObjectCollection) :
	id_{ _id },
	pGameObjectCollection_{ _pGameObjectCollection }
{
}

cha::GameObject::~GameObject()
{
}

cha::GameObject::operator bool() const
{
	return !IsEmpty() && pGameObjectCollection_->Exist(id_);
}
