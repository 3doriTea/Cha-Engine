#pragma once
#include "BasePch.h"

#include "Core/IObject.h"
#include "Core/IScene.h"
#include "Core/ISystemObject.h"
#include "Manager/IManager.h"

#include "Core/SceneBase.h"
#include "Core/SystemObjectBase.h"
#include "Core/GameObject.h"
#include "Core/GameObjectCollection.h"
#include "Core/Component.h"
#include "Core/ComponentBase.h"

#include "Core/Resource/IResourceRegistry.h"
#include "Core/Resource/ResourceRegistryBase.h"
#include "Core/Resource.h"

#include "Core/Resource/Audio.h"
#include "Core/Resource/Shader.h"
#include "Core/Resource/Material.h"
#include "Core/Resource/Surface.h"
#include "Core/Resource/Texture.h"
#include "Core/Resource/Model.h"
#include "Core/Resource/ShaderPro.h"

#include "Core/Resource/ShaderPro/ShaderProBuilder.h"

#include "EntryPoint/EntryPoint.h"

// JSONへの変換するやつ
namespace cha
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Float4, x, y, z, w)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec3, x, y, z)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec2, x, y)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec3Int, x, y, z)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec2Int, x, y)
}

using namespace cha;
