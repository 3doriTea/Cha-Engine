#include "MessageAuthor.h"

// TODO: 全く同じこと書いているんだからクラス化しよう

void MessageAuthor::Start()
{
	transform.position = begin;
	transform.scale = scale;
}

void MessageAuthor::Show()
{
	transform.scale.x = scale.x;
}

void MessageAuthor::Hide()
{
	transform.scale.x = 0.0f;
}
