#pragma once
#include <Game/Game.h>

/// @brief 水平ゲージUIのデータ
struct HorizontalGaugeData
{
	float maxValue{};      ///< ゲージの最大値
	float currentValue{};  ///< ゲージの現在の値

	Component<UITransform> transform{};  ///< UI座標系

	float frameWidthPx{};  ///< 縁の太さ(ピクセル)

	Color frameColor{};       ///< 縁の色
	Color backgroundColor{};  ///< ゲージ背景の色
	Color innerColor{};       ///< バーの色
};

/// @brief 水平ゲージUI
struct HorizontalGauge : ObjectBase<HorizontalGaugeData>
{
	using ObjectBase<HorizontalGaugeData>::ObjectBase;

	/// @brief 描画処理
	void Draw() override;
};
