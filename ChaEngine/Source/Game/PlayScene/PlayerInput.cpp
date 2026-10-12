
#include "PlayerInput.h"

void PlayerInput::Update()
{
	RawMouseInput& mouseInput = Get<RawMouseInput>();
	InputSystem& input = Get<InputSystem>();

	move = Vec3::Zero();

	if (input.GetKey(KeyCode::W))
		move += Vec3::Forward();
	if (input.GetKey(KeyCode::A))
		move += Vec3::Left();
	if (input.GetKey(KeyCode::S))
		move += Vec3::Back();
	if (input.GetKey(KeyCode::D))
		move += Vec3::Right();

	// 正規化して長さ1にする
	move = move.Normalize();

	// ジャンプの瞬間
	jumpOn = input.GetKeyDown(KeyCode::Space)
		|| input.GetKeyDown(KeyCode::LeftShift);

	// ジャンプ中
	jumping = input.GetKey(KeyCode::Space)
		|| input.GetKey(KeyCode::LeftShift);

	// ツールのメインアクションの瞬間
	toolActionMainOn = mouseInput.GetMouseDown(MouseCode_Left);
	// ツールのメインアクション中
	toolActionMain = mouseInput.GetMouse(MouseCode_Left);

	// ツールのメインアクションの瞬間
	toolActionSubOn = mouseInput.GetMouseDown(MouseCode_Right);
	// ツールのメインアクション中
	toolActionSub = mouseInput.GetMouse(MouseCode_Right);

	// ツールのスクロール
	toolScroll = mouseInput.GetScrollWheel();
}
