
#include "Stake.h"

void Stake::Start()
{
}

void Stake::Update()
{
	transform.position.x += Get<DeltaTime>();

	if (transform.position.x >= 5.0f)
	{
		DestroyMe();
	}
}
