#pragma once
#include <Game/Game.h>

/// @brief 字幕エリア内のテキスト本体のデータ
struct MessageTextData
{
	Component<UITransform> transform{};   // UI座標系
	Component<UIText> text{};             // 文字描画用

	Vec2 scale{ 940, 100 };  // テキスト本体のUIスケール
	Vec2 begin{ 340, 670 };  // テキスト本体のUI位置
};

/// @brief 字幕エリア内のテキスト本体
struct MessageText : ObjectBase<MessageTextData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 表示する
	void Show();

	/// @brief 非表示にする
	void Hide();
};
