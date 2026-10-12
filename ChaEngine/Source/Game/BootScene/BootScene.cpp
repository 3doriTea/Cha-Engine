#include "BootScene.h"
#include "BootSceneBackground.h"
#include "BootSceneLogo.h"
#include <Game/Common/GameManager.h>

void BootScene::Start()
{
	Instantiate<GameManager>();

	Instantiate<BootSceneLogo>();
	Instantiate<BootSceneBackground>();
}
