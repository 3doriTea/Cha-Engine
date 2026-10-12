
#include "Button.h"
#include "Button/ButtonText.h"

void UI::Button::Start()
{
	//objButtonText = GetScene().Instantiate<ButtonText>();
}

void UI::Button::Update()
{
	switch (target.currentState)
	{
	case UIRaycastTargetState_Normal:
		sprite.texture = textureNormal;
		break;
	case UIRaycastTargetState_Hovered:
		sprite.texture = textureHover;
		break;
	case UIRaycastTargetState_Pressed:
		sprite.texture = texturePush;
		break;
	default:
		break;
	}
}
