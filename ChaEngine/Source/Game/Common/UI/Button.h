#pragma once
#include <Game/Game.h>

namespace UI
{
	/// @brief ボタンUIオブジェクトのデータ
	struct ButtonData
	{
		ButtonData(
			const std::string_view _normal,
			const std::string_view _hover,
			const std::string_view _push,
			const std::string_view _content,
			const Vec2 _begin,
			const Vec2 _scale
		) :
			textureNormal{ _normal },
			textureHover{ _hover },
			texturePush{ _push },
			transform{ _begin, _scale },
			text{ _content }
		{}
	
		std::string_view content{};  // 表示内容

		Component<UITransform> transform{};   // UI座標系
		Component<UIImage> sprite{};          // UI画像
		Component<UIRaycastTarget> target{};  // 当たり判定用
		Component<UIText> text{};             // ボタンのテキスト
	
		Resource<ResourceType::Texture> textureNormal{};  // 普通のボタン
		Resource<ResourceType::Texture> textureHover{};   // カーソルが当たってるボタン
		Resource<ResourceType::Texture> texturePush{};    // 押されたボタン
	};
	
	/// @brief ボタンUIオブジェクト
	struct Button : ObjectBase<ButtonData>
	{
		using ObjectBase<ButtonData>::ObjectBase;

		/// @brief 開始処理
		void Start() override;
		/// @brief 更新処理
		void Update() override;

		/// @brief ボタンが押された瞬間かを取得
		/// @return ボタンが押された瞬間
		bool IsPushed() const { return target.isReleasedThisFrame; }
	};
}
