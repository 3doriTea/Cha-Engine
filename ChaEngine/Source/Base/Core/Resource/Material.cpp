
#include "Material.h"
#include "../../ManagerRegistry.h"
#include "../../Manager/Dx11Manager.h"
#include "Material/IMaterialProperty.h"

bool cha::MaterialResource::TryLoad(std::unique_ptr<IMaterialProperty>&& _pProperty)
{
	pProperty = std::move(_pProperty);

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth           = pProperty->GetSize(),
		.Usage               = D3D11_USAGE_DYNAMIC,
		.BindFlags           = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags           = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref([this, &BUFFER_DESC, &hResult](const ComPtr<ID3D11Device>& _pDevice)
	{
		hResult = _pDevice->CreateBuffer(&BUFFER_DESC, nullptr, pConstantBuffer.GetAddressOf());
	});

	VCU_ASSERT(SUCCEEDED(hResult)
		&& "コンスタントバッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}
