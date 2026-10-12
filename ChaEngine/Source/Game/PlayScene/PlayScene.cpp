
#include "PlayScene.h"
#include "Player.h"
#include "PlayerInput.h"
#include "PlayerCamera.h"
#include "PlayerCamera/CameraSpectator.h"
#include "PlayerCamera/CameraTPS.h"
#include "../Common/GameManager.h"
#include "../Common/Sky.h"
#include "../TestScene/SceneTester.h"
#include "TestAlphaCube.h"

#include "Terrain.h"
#include "DebugCamera.h"

#include "UI/MessageArea.h"

void PlayScene::Start()
{
	Instantiate<GameManager>();

	Instantiate<SceneTester>("PlayScene");

	GameObject objTerrain = Instantiate<Terrain>();

	Instantiate<DebugCamera>();

	Instantiate<TestAlphaCube>();
	GameObject objMessageArea = Instantiate<MessageArea>();

	GameObject objPlayerInput = Instantiate<PlayerInput>();
	GameObject objPlayer = Instantiate<Player>(objPlayerInput, objTerrain, objMessageArea);


	Instantiate<PlayerCamera>(objPlayer, std::array<GameObject, 2>
	{
		Instantiate<CameraSpectator>(),
		Instantiate<CameraTPS>(objPlayer, objTerrain),
	});

	Instantiate<Sky>();
}
