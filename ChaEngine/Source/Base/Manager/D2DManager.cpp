
#include "D2DManager.h"
#include <Base/Manager/Dx11Manager.h>

cha::D2DManager::D2DManager(Dx11Manager& _dx11) :
	dx11_{ _dx11 }
{
}

bool cha::D2DManager::Initialize()
{
	HRESULT hResult{};

	// Direct2DとDirectWriteのファクトリ作成
	D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
	// デバッグはかなり詳細に
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	hResult = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1), &options,
		reinterpret_cast<void**>(pD2DFactory_.ReleaseAndGetAddressOf()));
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "D2D1の作成に失敗");
		return false;
	}

	// NOTE: IUnknown** な点注意
	ComPtr<IUnknown> pUnknownFactory{};
	hResult = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory3),
		pUnknownFactory.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "IDWriteFactoryの作成に失敗");
		return false;
	}

	// pDWriteFactory_へキャストして渡す
	hResult = pUnknownFactory.As(&pDWriteFactory_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "IDWriteFactoryのキャストに失敗");
		return false;
	}

	ComPtr<IDXGIDevice> pDxgiDevice{};
	dx11_.Ref([&hResult, &pDxgiDevice](const ComPtr<ID3D11Device>& _pDevice)
	{
		hResult = _pDevice->QueryInterface(
			__uuidof(IDXGIDevice),
			reinterpret_cast<void**>(pDxgiDevice.ReleaseAndGetAddressOf()));
	});
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "pDxgiDeviceの取得に失敗");
		return false;
	}

	hResult = pD2DFactory_->CreateDevice(pDxgiDevice.Get(), pD2DDevice_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "pD2DFactory_->CreateDeviceに失敗");
		return false;
	}

	hResult = pD2DDevice_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, pD2DContext_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "pD2DDevice_->CreateDeviceContextに失敗");
		return false;
	}

	// --- Step 3: スワップチェーンのバックバッファを D2D のレンダーターゲットとして紐付ける ---
	ComPtr<IDXGISurface> pBackBuffer{};
	dx11_.RefSwapChain([&hResult, &pBackBuffer](const ComPtr<IDXGISwapChain>& _pSwapChain)
	{
		hResult = _pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), &pBackBuffer);
	});
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "_pSwapChain->GetBufferに失敗");
		return false;
	}

	// DPIの取得とピクセルプロパティの設定
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	hResult = pD2DContext_->CreateBitmapFromDxgiSurface(pBackBuffer.Get(), &bitmapProperties, &pD2DTargetBitmap_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "CreateBitmapFromDxgiSurfaceに失敗");
		return false;
	}

	// D2Dの描画先を設定
	pD2DContext_->SetTarget(pD2DTargetBitmap_.Get());

	//// 7. DirectWrite テキストフォーマットの作成 (フォント名、サイズ等の指定)
	//HRESULT hResult = pDWriteFactory_->CreateTextFormat(
	//	L"Meiryo",                // フォントファミリ名
	//	nullptr,                  // フォントコレクション (nullptrでシステムフォント)
	//	DWRITE_FONT_WEIGHT_NORMAL,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	24.0f,                    // フォントサイズ (DIP単位)
	//	L"ja-jp",                 // ロケール
	//	&pTextFormat_
	//);
	//if (FAILED(hResult))
	//{
	//	VCU_ASSERT(SUCCEEDED(hResult) && "CreateTextFormatに失敗");
	//	return hResult;
	//}
	// フォントの設定
	hResult = InitializeLocalFont();
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "InitializeLocalFontに失敗");
		return false;
	}

	// テキストの整列設定
	pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	//pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	InitializeBrushes();

	return true;
}

HRESULT cha::D2DManager::InitializeBrushes()
{
	HRESULT hResult;
	hResult = pD2DContext_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pTextBrush_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "CreateSolidColorBrushに失敗");
		return hResult;
	}

	hResult = pD2DContext_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pFigureBrush_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "図形描画用 CreateSolidColorBrushに失敗");
		return hResult;
	}

	return S_OK;
}

void cha::D2DManager::Begin()
{
	if (pD2DContext_)
	{
		pD2DContext_->BeginDraw();
	}
}

void cha::D2DManager::End()
{
	if (pD2DContext_)
	{
		// EndDrawの戻り値でデバイスロスト等のハンドリングが可能
		HRESULT hResult = pD2DContext_->EndDraw();
		if (hResult == D2DERR_RECREATE_TARGET)
		{
			// 必要に応じてリソースの再生成処理をここに記述
			VCU_ASSERT(false && "リソースの再生成処理");
		}
	}
}

void cha::D2DManager::DrawString(const std::wstring_view _text, const Vec2 _begin, const Vec2 _size)
{
	if (!pD2DContext_)
	{
		return;  // コンテキストがないなら無視
	}

	if (_text.empty())
	{
		return;  // 空っぽな文字列は描画するだけ無駄なため無視
	}

	D2D1_RECT_F renderRect = D2D1::RectF(_begin.x, _begin.y, _begin.x + _size.x, _begin.y + _size.y);

	// いざ描画
	pD2DContext_->DrawTextW(
		_text.data(),
		static_cast<UINT32>(_text.size()),
		pTextFormat_.Get(),
		renderRect,
		pTextBrush_.Get());
}

void cha::D2DManager::DrawString(const std::string_view _text, const Vec2 _begin, const Vec2 _size)
{
	if (_text.empty())
	{
		return;  // 空っぽな文字列は描画するだけ無駄なため無視
	}

	int neededSize = MultiByteToWideChar(CP_UTF8, 0, _text.data(), static_cast<int>(_text.size()), nullptr, 0);

	// TODO: この変換もこのメモリも無駄なため文字列セットを作って管理するといいかも
	std::wstring wstr(neededSize, 0);

	MultiByteToWideChar(CP_UTF8, 0, _text.data(), static_cast<int>(_text.size()), &wstr[0], neededSize);

	DrawString(wstr, _begin, _size);
}

HRESULT cha::D2DManager::InitializeLocalFont()
{
	HRESULT hResult = S_OK;

	// TODO: マジックを消す
	const fs::path FONT_FILE{ "Font/YuseiMagic/YuseiMagic-Regular.ttf" };

	// 1. フォントファイルの参照を作成 (例: 実行ファイルと同じ場所の "myfont.ttf")
	ComPtr<IDWriteFontFile> pFontFile{};
	hResult = pDWriteFactory_->CreateFontFileReference(FONT_FILE.c_str(), nullptr, &pFontFile);
	if (FAILED(hResult))
	{
		return hResult;
	}

	ComPtr<IDWriteFontFaceReference> pFontFaceReference{};
	hResult = pDWriteFactory_->CreateFontFaceReference(
		pFontFile.Get(),
		0,                               // faceIndex (通常は0)
		DWRITE_FONT_SIMULATIONS_NONE,    // フォントのシミュレーション（斜体や太字の強制変形なし）
		&pFontFaceReference
	);
	if (FAILED(hResult))
	{
		return hResult;
	}

	// 2. フォントセットビルダーを使って、このファイルを含むフォントセットを作成
	ComPtr<IDWriteFontSetBuilder> pFontSetBuilder{};
	hResult = pDWriteFactory_->CreateFontSetBuilder(&pFontSetBuilder);
	if (FAILED(hResult))
	{
		return hResult;
	}

	hResult = pFontSetBuilder->AddFontFaceReference(pFontFaceReference.Get());
	if (FAILED(hResult))
	{
		return hResult;
	}

	ComPtr<IDWriteFontSet> pFontSet{};
	hResult = pFontSetBuilder->CreateFontSet(&pFontSet);
	if (FAILED(hResult))
	{
		return hResult;
	}

	// 3. フォントセットからフォントコレクションを作成
	ComPtr<IDWriteFontCollection1> pFontCollection{};
	hResult = pDWriteFactory_->CreateFontCollectionFromFontSet(pFontSet.Get(), &pFontCollection);
	if (FAILED(hResult))
	{
		return hResult;
	}

	// 4. 作成したフォントコレクションを渡して TextFormat を作成
	// ※フォントファミリ名は、システム名ではなく「フォントファイルが内部で持っている正式名」を指定します
	hResult = pDWriteFactory_->CreateTextFormat(
		L"Yusei Magic",               // NOTE: フォント内の正確なファミリ名でなければいけない
		pFontCollection.Get(),
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32.0f,
		L"ja-jp",
		pTextFormat_.GetAddressOf()
	);

	return hResult;
}
