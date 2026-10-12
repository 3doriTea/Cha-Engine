
#include "UIRaycasterSystem.h"
#include "../Component/UI/UITransform.h"
#include "../Component/UI/UIRaycastTarget.h"

void UIRaycasterSystem::Update()
{
	auto view = GetView<UITransform, UIRaycastTarget>();

	view.ForEach([](IObject& _object, UITransform& _transform, UIRaycastTarget& _target)
	{
		
	});
}
