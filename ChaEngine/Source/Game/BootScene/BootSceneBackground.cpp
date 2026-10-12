#include "BootSceneBackground.h"
#include <Game/Common/Canvas.h>

void BootSceneBackground::Start()
{
	transform.scale = Get<ScreenSize>();
}

void BootSceneBackground::Draw()
{
	Get<Canvas>().DrawBox(transform, Color::FromWebCode(0x578EF5));
}
