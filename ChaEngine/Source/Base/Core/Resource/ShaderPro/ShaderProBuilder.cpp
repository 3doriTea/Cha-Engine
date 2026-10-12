
#include "ShaderProBuilder.h"
#include "ShaderProStepVertex.h"
#include "ShaderProStepPixel.h"
#include "ShaderProStepGeometry.h"
#include "../../../ManagerRegistry.h"
#include "../../../Manager/Dx11Manager.h"

cha::ShaderProBuilder::ShaderProBuilder(
	const fs::path& _file,
	const ShaderCompileConfig& _compileConfig) :
	FILE_{ _file },
	COMPILE_CONFIG_{ _compileConfig }
{
}

cha::ShaderProBuilder& cha::ShaderProBuilder::AddVertexShader()
{
	pSteps_.emplace_back(std::make_unique<ShaderProStepVertex>());
	bool succeed = pSteps_.back()->TryLoad(
		ManagerRegistry::Ref<Dx11Manager>(),
		FILE_,
		COMPILE_CONFIG_.pInclude,
		COMPILE_CONFIG_.vertexEntryName,
		COMPILE_CONFIG_.vertexVersion,
		COMPILE_CONFIG_.flags);

	VCU_ASSERT(succeed && "頂点シェーダの読み込みに失敗");

	return *this;
}

cha::ShaderProBuilder& cha::ShaderProBuilder::AddPixelShader(const RasterizerConfig& _config)
{
	pSteps_.emplace_back(std::make_unique<ShaderProStepPixel>(_config));
	bool succeed = pSteps_.back()->TryLoad(
		ManagerRegistry::Ref<Dx11Manager>(),
		FILE_,
		COMPILE_CONFIG_.pInclude,
		COMPILE_CONFIG_.pixelEntryName,
		COMPILE_CONFIG_.pixelVersion,
		COMPILE_CONFIG_.flags);

	VCU_ASSERT(succeed && "ピクセルシェーダの読み込みに失敗");

	return *this;
}

cha::ShaderProBuilder& cha::ShaderProBuilder::AddGeometryShader()
{
	pSteps_.emplace_back(std::make_unique<ShaderProStepGeometry>());
	bool succeed = pSteps_.back()->TryLoad(
		ManagerRegistry::Ref<Dx11Manager>(),
		FILE_,
		COMPILE_CONFIG_.pInclude,
		COMPILE_CONFIG_.geometryEntryName,
		COMPILE_CONFIG_.geometryVersion,
		COMPILE_CONFIG_.flags);

	VCU_ASSERT(succeed && "ジオメトリシェーダの読み込みに失敗");

	return *this;
}
