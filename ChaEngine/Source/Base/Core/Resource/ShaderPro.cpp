
#include "ShaderPro.h"
#include "../../ManagerRegistry.h"
#include "../../Manager/Dx11Manager.h"

bool cha::ShaderProRegistry::TryLoad(ShaderProLoadConfig&& _config, ShaderProResource& _out)
{
	// 全部まとめて移動
	_out.steps = std::move(_config.builder.pSteps_);
	return true;
}
