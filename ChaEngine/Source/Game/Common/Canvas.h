#pragma once
#include <Game/Game.h>

/// @brief 2D描画用のキャンバスデータ
struct CanvasData
{
	ComPtr<ID2D1SolidColorBrush> pFigureBrush{};  // 図形描画用ブラシ
};

/// @brief 2D描画用のキャンバス
struct Canvas : SystemObjectBase<Canvas, CanvasData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 描画図形をラジアンで回転させる
	/// @param _layerOrder 描画レイヤー
	/// @param _angleRad 回転角度(ラジアン)
	/// @param _pivot 回転の軸オフセット
	void SetAngle(
		const UILayerOrder _layerOrder,
		const float _angleRad = 0.0f,
		const Vec2 _pivot = {});

	/// @brief 2D描画をする
	void Draw(std::function<void(
		const ComPtr<ID2D1Factory1>&,
		const ComPtr<ID2D1DeviceContext>&,
		const ComPtr<ID2D1SolidColorBrush>&)> _func,
		const UILayerOrder _layerOrder);
	
	/// @brief 線分を描画する
	/// @param _begin 線分の始点
	/// @param _end 線分の終点
	/// @param _color 線分の色
	/// @param _layerOrder 描画レイヤー
	/// @param _thickness 線分の太さ
	void DrawLine(
		const Vec2 _begin,
		const Vec2 _end,
		const Color _color,
		const UILayerOrder _layerOrder,
		const float _thickness = 1.0f);

	/// @brief 三角形を描画する
	/// @param _v1 頂点1
	/// @param _v2 頂点2
	/// @param _v3 頂点3
	/// @param _color 色
	/// @param _layerOrder 描画レイヤー
	void DrawTriangle(
		const Vec2 _v1,
		const Vec2 _v2,
		const Vec2 _v3,
		const Color _color,
		const UILayerOrder _layerOrder);

	/// @brief 矩形を描画する
	/// @param _begin 矩形の始点
	/// @param _end 矩形の終点
	/// @param _color 色
	/// @param _layerOrder 描画レイヤー
	void DrawBox(
		const Vec2 _begin,
		const Vec2 _end,
		const Color _color,
		const UILayerOrder _layerOrder);

	/// @brief UITransformの矩形を描画する
	/// @param _transform UITransform
	/// @param _color 色
	void DrawBox(const UITransform& _transform, const Color _color);
};
