#pragma once
#include <Game/Game.h>

/// @brief 字幕エリア内の発言者の本体のデータ
struct MessageAuthorData
{
	Component<UITransform> transform{};   // UI座標系
	Component<UIText> text{};             // 文字描画用

	Vec2 scale{ 350, 40 };  // 発言者のUIスケール
	Vec2 begin{ 330, 600 };  // 発言者のUI位置
};

/// @brief 字幕エリア内の発言者
struct MessageAuthor : ObjectBase<MessageAuthorData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 表示する
	void Show();

	/// @brief 非表示にする
	void Hide();
};
