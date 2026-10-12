#include "BootSceneLogo.h"
#include <Game/PlayScene/PlayScene.h>

void BootSceneLogo::Start()
{
	const Vec2 SCREEN_SIZE = Get<ScreenSize>();
	transform.position = (SCREEN_SIZE - transform.scale) * 0.5f;
	image.texture = texture;
}

void BootSceneLogo::Update()
{
	image.baseColor = Color::FromWebCodeA(0xffffff00);

	timeLeftSec -= Get<DeltaTime>();

	float last = timeLeftSec;

	if (last <= 0.0f)
	{
		// ロゴの表示が終わったためシーン遷移する
		Get<SceneManager>().Move<PlayScene>();
		return;
	}

	last -= fadeOutTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = 1.0f - -last / fadeOutTime;
		return;
	}

	last -= stayTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = 1.0f;
		return;
	}

	last -= fadeInTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = -last / fadeOutTime;
		return;
	}

	ImGui::End();
}
