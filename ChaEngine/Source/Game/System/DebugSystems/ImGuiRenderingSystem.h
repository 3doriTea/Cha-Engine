#pragma once
#include <Base/Base.h>

/// @brief ImGuiで描画するシステムのデータ
struct ImGuiRenderingSystemData
{

};

/// @brief ImGuiで描画するシステム
struct ImGuiRenderingSystem : SystemObjectBase<ImGuiRenderingSystem, ImGuiRenderingSystemData>
{
	/// @brief 毎フレームの更新処理
	void Update() override;
};
