
#include "TestScene.h"
#include "../Common/GameManager.h"
#include "SceneTester.h"

void TestScene::Start()
{
	Instantiate<GameManager>();

	Instantiate<SceneTester>("TestScene");
}
