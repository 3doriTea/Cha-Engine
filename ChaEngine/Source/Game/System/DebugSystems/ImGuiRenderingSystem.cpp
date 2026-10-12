
#include "ImGuiRenderingSystem.h"
#include <Game/Component/Name.h>
#include <Game/Component/ImTransform.h>
#include <Game/Component/Transform.h>

void ImGuiRenderingSystem::Update()
{
	// Transform表示用のビュー
	auto view = GetView<Transform, ImTransform, Name>();

	view.ForEach([](IObject& _obj, Transform& _transform, ImTransform&, Name& _name)
	{
		ImGui::Begin(std::format("Transform{{{}}}", _name.content).c_str());

		ImGui::DragFloat3("position", _transform.position.v);

		Vec3 angles = _transform.rotation.ToEuler();
		ImGui::DragFloat3("rotation", angles.v);
		_transform.rotation = Quaternion::FromEuler(angles);

		ImGui::DragFloat3("scale", _transform.scale.v);

		ImGui::End();
	});
}
