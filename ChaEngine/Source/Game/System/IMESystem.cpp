#include "IMESystem.h"
#include <Base/Manager/WinMessageManager.h>

void IMESystem::Start()
{
}

void IMESystem::Update()
{
	// TODO: とくに文字入力使わない！
	/*if (Get<InputSystem>().GetKeyDown(KeyCode::Enter))
	{
		Get<WinMessageManager>().BeginIME();
	}*/

	//Get<WinMessageManager>().
	//OutputDebugString(std::format(L"{}\r\n", Get<WinMessageManager>().GetIME()).c_str());
}
