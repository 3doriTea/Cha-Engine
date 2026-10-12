#include "MessageText.h"

void MessageText::Start()
{
	transform.position = begin;
	transform.scale = scale;
}

void MessageText::Show()
{
	transform.scale.x = scale.x;
}

void MessageText::Hide()
{
	transform.scale.x = 0.0f;
}
