#pragma once
#include "../BasePch.h"
#include "IManager.h"

namespace cha
{
	class Dx11Manager;

	/// @brief Direct2D のマネージャ
	class D2DManager : public IManager
	{
	public:
		D2DManager(Dx11Manager& _dx11);

		/// @brief 初期化処理
		/// @return 初期化に成功 true / false
		bool Initialize();

		/// @brief ブラシの初期化
		/// @return 結果型
		HRESULT InitializeBrushes();

		/// @brief 2Dの描画開始
		void Begin();

		/// @brief 2Dの描画終了
		void End();

		/// @brief 文字列を描画する
		/// @param _text 文字列
		/// @param _begin 左上の座標
		/// @param _size 左上からのサイズ
		void DrawString(const std::wstring_view _text, const Vec2 _begin, const Vec2 _size);
		/// @brief 文字列を描画する
		/// @param _text 文字列
		/// @param _begin 左上の座標
		/// @param _size 左上からのサイズ
		void DrawString(const std::string_view _text, const Vec2 _begin, const Vec2 _size);

		/// @brief ローカルフォルダにあるフォントを読み込む
		HRESULT InitializeLocalFont();

		/// @brief ID2D1Factory1 / ID2D1DeviceContext を参照する
		/// @tparam Func Any(const ComPtr<ID2D1Factory1>&, const ComPtr<ID2D1DeviceContext>&)
		/// @param _func 任意の戻り値を返すラムダ式
		/// @return Any(任意の型)
		template<typename Func>
		inline decltype(auto) Ref(Func&& _func)
		{
			if constexpr (std::is_invocable_v<Func, decltype(pD2DFactory_), decltype(pD2DContext_)>)
			{
				return _func(pD2DFactory_, pD2DContext_);
			}
			else if constexpr (std::is_invocable_v<Func, decltype(pD2DContext_), decltype(pD2DFactory_)>)
			{
				return _func(pD2DContext_, pD2DFactory_);
			}
			else if constexpr (std::is_invocable_v<Func, decltype(pD2DFactory_)>)
			{
				return _func(pD2DFactory_);
			}
			else if constexpr (std::is_invocable_v<Func, decltype(pD2DContext_)>)
			{
				return _func(pD2DContext_);
			}
			else
			{
				static_assert(
					dependent_false<Func>,
					"ComPtr<ID2D1Factory1>かComPtr<ID2D1DeviceContext>またはその両方を受け取る引数を用意してください。");
			}
		}

	private:
		Dx11Manager& dx11_;  // directx11の参照

		ComPtr<ID2D1Factory1> pD2DFactory_;          // d2dファクトリ v1 (direct x 11連携強化版)
		ComPtr<ID2D1Device> pD2DDevice_;             // デバイス
		ComPtr<ID2D1DeviceContext> pD2DContext_;     // デバイスコンテキスト
		ComPtr<ID2D1Bitmap1> pD2DTargetBitmap_;      // 描画先
		ComPtr<IDWriteFactory3> pDWriteFactory_;     // direct write のファクトリ
		ComPtr<IDWriteTextFormat> pTextFormat_;      // テキストフォーマット
		ComPtr<ID2D1SolidColorBrush> pTextBrush_;    // テキストのブラシ
		ComPtr<ID2D1SolidColorBrush> pFigureBrush_;  // 図形描画用ブラシ
	};
}
