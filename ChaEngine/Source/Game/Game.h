#pragma once
#include <Base/Base.h>

#include <Base/Manager/DeltaTime.h>
#include <Base/Manager/LightManager.h>
#include <Base/Manager/RawMouseInput.h>

#include <Base/Core/Mathf.h>
#include <Base/Core/SceneBase.inl>

#include "System/CameraSystem.h"
#include "System/InputSystem.h"
#include "System/CursorSystem.h"

#include "Component/Name.h"
#include "Component/Transform.h"
#include "Component/DynamicTransform.h"
#include "Component/UI/UITransform.h"
#include "Component/UI/UIImage.h"
#include "Component/UI/UIText.h"
#include "Component/UI/UIRaycastTarget.h"
#include "Component/ImTransform.h"
#include "Component/StaticOBB.h"
#include "Component/DynamicOBB.h"
#include "Component/OBBRenderer.h"
#include "Component/RigidBody.h"

#include "Utility/File/JsonLoader.h"

using enum ResourceType;  // ResourceType::と書かなくて良い
