#include "Canvas.h"
#include <Base/Manager/UIManager.h>
#include <Base/Manager/D2DManager.h>

void Canvas::Start()
{
	HRESULT hResult = Get<D2DManager>().Ref(
		[this](const ComPtr<ID2D1DeviceContext>& _pContext) -> HRESULT
	{
		return _pContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), pFigureBrush.GetAddressOf());
	});

	VCU_ASSERT(SUCCEEDED(hResult) && "図形用カラーブラシの作成に失敗");
}

void Canvas::SetAngle(
	const UILayerOrder _layerOrder,
	const float _angleRad,
	const Vec2 _pivot)
{
	Get<UIManager>().AddRenderer([this, _angleRad, _pivot]
	{
		Get<D2DManager>().Ref(
			[this, _angleRad, _pivot](const ComPtr<ID2D1DeviceContext>& _pContext)
		{
			static const float PI{ std::acosf(-1.0f) };
			static const float RAD_TO_DEG{ 180.0f / PI };

			// ラジアンで回転を適用
			_pContext->SetTransform(
				D2D1::Matrix3x2F::Rotation(
					RAD_TO_DEG * _angleRad,
					D2D1::Point2F(_pivot.x, _pivot.y)));
		});
	}, _layerOrder.WithFuncType(UILayerOrder::FuncType_Preparation), true);
}

void Canvas::Draw(
	std::function<void(
		const ComPtr<ID2D1Factory1>&,
		const ComPtr<ID2D1DeviceContext>&,
		const ComPtr<ID2D1SolidColorBrush>&)> _func,
		const UILayerOrder _layerOrder)
{
	Get<UIManager>().AddRenderer([this, _func]
	{
		Get<D2DManager>().Ref([this, _func]
		(const ComPtr<ID2D1Factory1>& _pFactory, const ComPtr<ID2D1DeviceContext>& _pContext)
		{
			_func(_pFactory, _pContext, pFigureBrush);
		});
	}, _layerOrder.WithFuncType(UILayerOrder::FuncType_Rendering), true);
}

void Canvas::DrawLine(
	const Vec2 _begin,
	const Vec2 _end,
	const Color _color,
	const UILayerOrder _layerOrder,
	const float _thickness)
{
	Draw([_begin, _end, _color, _thickness](
		const ComPtr<ID2D1Factory1>& _pFactory,
		const ComPtr<ID2D1DeviceContext>& _pContext,
		const ComPtr<ID2D1SolidColorBrush>& _pBrush)
	{
		_pBrush->SetColor(D2D1::ColorF{ _color.r, _color.g, _color.b, _color.a });

		// 線分を描画する
		_pContext->DrawLine(
			D2D1::Point2F(_begin.x, _begin.y),
			D2D1::Point2F(_end.x, _end.y),
			_pBrush.Get(),
			_thickness);
	}, _layerOrder.WithUIType(UILayerOrder::UIType_Figure));
}

void Canvas::DrawTriangle(
	const Vec2 _v1,
	const Vec2 _v2,
	const Vec2 _v3,
	const Color _color,
	const UILayerOrder _layerOrder)
{
	Draw([_v1, _v2, _v3, _color](
		const ComPtr<ID2D1Factory1>& _pFactory,
		const ComPtr<ID2D1DeviceContext>& _pContext,
		const ComPtr<ID2D1SolidColorBrush>& _pBrush)
	{
		_pBrush->SetColor(D2D1::ColorF{ _color.r, _color.g, _color.b, _color.a });

		// パスを作る
		ComPtr<ID2D1PathGeometry1> pPathGeometry{};
		_pFactory->CreatePathGeometry(pPathGeometry.GetAddressOf());

		// ジオメトリに形を書き込む
		ComPtr<ID2D1GeometrySink> pSink{};
		pPathGeometry->Open(&pSink);

		// 3点の頂点
		D2D1_POINT_2F p1{ D2D1::Point2F(_v1.x, _v1.y) };
		D2D1_POINT_2F p2{ D2D1::Point2F(_v2.x, _v2.y) };
		D2D1_POINT_2F p3{ D2D1::Point2F(_v3.x, _v3.y) };

		// 一筆書き開始
		pSink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);

		pSink->AddLine(p2);
		pSink->AddLine(p3);

		// 一筆書き終了
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		pSink->Close();  // 形を確定

		// 塗りつぶし描画
		_pContext->FillGeometry(pPathGeometry.Get(), _pBrush.Get());
	}, _layerOrder.WithUIType(UILayerOrder::UIType_Figure));
}

void Canvas::DrawBox(
	const Vec2 _begin,
	const Vec2 _end,
	const Color _color,
	const UILayerOrder _layerOrder)
{
	Draw([_begin, _end, _color](
		const ComPtr<ID2D1Factory1>& _pFactory,
		const ComPtr<ID2D1DeviceContext>& _pContext,
		const ComPtr<ID2D1SolidColorBrush>& _pBrush)
	{
		_pBrush->SetColor(D2D1::ColorF{ _color.r, _color.g, _color.b, _color.a });

		// 矩形を描画する
		_pContext->FillRectangle(D2D1::RectF(_begin.x, _begin.y, _end.x, _end.y), _pBrush.Get());
	}, _layerOrder.WithUIType(UILayerOrder::UIType_Figure));
}

void Canvas::DrawBox(const UITransform& _transform, const Color _color)
{
	Draw([_transform, _color](
		const ComPtr<ID2D1Factory1>& _pFactory,
		const ComPtr<ID2D1DeviceContext>& _pContext,
		const ComPtr<ID2D1SolidColorBrush>& _pBrush)
	{
		_pBrush->SetColor(D2D1::ColorF{ _color.r, _color.g, _color.b, _color.a });

		// 矩形を描画する
		_pContext->FillRectangle(D2D1::RectF(
			_transform.position.x,
			_transform.position.y,
			_transform.position.x + _transform.scale.x,
			_transform.position.y + _transform.scale.y),
			_pBrush.Get());
	}, _transform.layerOrder.WithUIType(UILayerOrder::UIType_Figure));
}
