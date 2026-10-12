
#include "SceneTester.h"
#include "../TestScene/TestScene.h"
#include "../PlayScene/PlayScene.h"

#include <Base/Core/NullObject.h>
#include <Game/Common/Canvas.h>

void SceneTester::Start()
{
	transform.position = { 40, 40 };
	//transform.position = { 0, 0 };
	transform.scale = { 400, 80 };
	//transform.scale = { 1600, 900 };
	text.content = currentScene;
	pLifeChecker->SetName(currentScene);
}

void SceneTester::Update()
{
	if (Get<InputSystem>().GetKeyDown(KeyCode::F))
	{
		if (currentScene == "PlayScene")
		{
			Get<SceneManager>().Move<TestScene>();
		}
		else
		{
			Get<SceneManager>().Move<PlayScene>();
		}
	}
}

void SceneTester::Draw()
{
	Get<Canvas>().DrawBox(transform, Color::FromWebCodeA(0xffffff77));
}
