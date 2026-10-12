
#include "ShaderTimeSystem.h"
#include "ShaderTimeSystem/TimeMaterialProperty.h"

void ShaderTimeSystem::Start()
{
	timeMaterial = MaterialResource{ 5 };
	timeMaterial.Get().SetProperty<TimeMaterialProperty>();
}

void ShaderTimeSystem::Update()
{
	using namespace DirectX;

	// 時刻にデルタタイムをひたすら加算していく
	time += Get<DeltaTime>();

	MaterialResource& material = timeMaterial.Get();

	TimeMaterialProperty* pTimeMaterialProperty =
		static_cast<TimeMaterialProperty*>(material.pProperty.get());

	(*pTimeMaterialProperty)->time.x = time;

	// 時刻マテリアルをシェーダに送信
	Get<Dx11Manager>().SetMaterial(timeMaterial);
}
