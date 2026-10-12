
#include "GameLoop.h"

#include "Core/ResourceManagerInstance.h"

#include "Manager/GameTimeManager.h"
#include "Manager/WinMessageManager.h"
#include "Manager/WinWindowManager.h"
#include "Manager/Dx11Manager.h"
#include "Manager/SceneManager.h"
#include "Manager/DeltaTime.h"
#include "Manager/ScreenSize.h"
#include "Manager/IGameLoopCallable.h"
#include "Manager/IssueManager.h"
#include "Manager/ImGuiManager.h"
#include "Manager/LightManager.h"
#include "Manager/RawMouseInput.h"
#include "Manager/UIManager.h"
#include "Manager/D2DManager.h"

#include "../Resource.h"

#include "../Game/BootScene/BootScene.h"
#include "../Game/Common/VirtualInputManager.h"
#include "../Game/Common/ScoreManager.h"

#include "../Game/Network/NetworkSessionManager.h"
#include "../Game/Manager/SessionManager.h"

// Discord RPC機能使うならマクロ定義する
//#define USE_DISCORD_RPC

#ifdef USE_DISCORD_RPC
#include "../Game/Manager/DiscordRPC/DiscordManager.h"
#endif

#include "ManagerRegistry.h"

cha::GameLoop::GameLoop(const ClientConfig _clientConfig) :
	clientConfig_{ _clientConfig }
{
}

void cha::GameLoop::Run(const HINSTANCE _hInstance)
{
	HRESULT hResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	VCU_ASSERT(SUCCEEDED(hResult) && "CoInitializeExに失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗なら即リターン
	}

	IssueManager& issueManager = ManagerRegistry::Add<IssueManager>();

	GameTimeManager& gameTime = ManagerRegistry::Add<GameTimeManager>(
		clientConfig_.targetFPS,
		ManagerRegistry::Add<DeltaTime>());

	WinMessageManager& winMessage = ManagerRegistry::Add<WinMessageManager>(
		_hInstance);

	constexpr size_t MAX_LOADSTRING{ 100 };

	std::array<wchar_t, MAX_LOADSTRING> titleWStringBuffer{};
	std::array<wchar_t, MAX_LOADSTRING> appNameWStringBuffer{};
	LoadStringW(_hInstance, IDS_APP_TITLE, titleWStringBuffer.data(), MAX_LOADSTRING);
	LoadStringW(_hInstance, IDC_DIESHACLIENT, appNameWStringBuffer.data(), MAX_LOADSTRING);

	bool succeed{};

	ManagerRegistry::Add<ScreenSize>(clientConfig_.clientSize);

	WinWindowManager& winWindow = ManagerRegistry::Add<WinWindowManager>(
		clientConfig_.clientSize,
		clientConfig_.isWindow,
		_hInstance,
		winMessage.GetWndProc(),
		titleWStringBuffer.data(),
		appNameWStringBuffer.data());

	succeed = winWindow.Show();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "ウィンドウ表示に失敗");
		return;
	}

	Dx11Manager& dx11Manager = ManagerRegistry::Add<Dx11Manager>(
		winWindow.GetWndHandle(),
		clientConfig_.clientSize,
		clientConfig_.shadowMapSize,
		clientConfig_.isWindow);

	succeed = dx11Manager.Initialize();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "ウィンドウ表示に失敗");
		return;
	}

	RawMouseInput& rawMouseInput = ManagerRegistry::Add<RawMouseInput>(winWindow);

	succeed = rawMouseInput.Initialize();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "カーソルの生データマネージャの初期化処理に失敗");
		return;
	}

	LightManager& lightManager = ManagerRegistry::Add<LightManager>(
		dx11Manager);

	UIManager& uiManager = ManagerRegistry::Add<UIManager>();

	D2DManager& d2dManager = ManagerRegistry::Add<D2DManager>(dx11Manager);
	succeed = d2dManager.Initialize();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "D2Dの初期化処理に失敗");
		return;
	}

	ImGuiManager& imguiManager = ManagerRegistry::Add<ImGuiManager>(
		winWindow,
		dx11Manager);

	succeed = imguiManager.Initialize();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "ImGui初期化に失敗");
		return;
	}

	VirtualInputManager& virtualInputManager = ManagerRegistry::Add<VirtualInputManager>();
	succeed = virtualInputManager.Initialize();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "ボタン入力マネージャーの初期化に失敗");
		return;
	}

	SceneManager& sceneManager = ManagerRegistry::Add<SceneManager>();
	// ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
	// 最初に遷移するシーンはここで変更
	sceneManager.Move<BootScene>();
	// ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

	ScoreManager& scoreManager = ManagerRegistry::Add<ScoreManager>(
		"./score.json");
	scoreManager.Load();

	NetworkSessionManager& networkSessionManager = ManagerRegistry::Add<NetworkSessionManager>();

	SessionManager& sessionManager = ManagerRegistry::Add<SessionManager>();

#ifdef USE_DISCORD_RPC
	DiscordManager& discordManager = ManagerRegistry::Add<DiscordManager>();
	succeed = discordManager.Initialize();
	if (!succeed)
	{
		VCU_ASSERT(!succeed && "DiscordManagerの初期化に失敗");
		return;
	}
#endif

	IGameLoopCallable& callableSceneManager{ sceneManager };

	while (true)
	{
		if (issueManager.HasError())
		{
			const std::wstring_view LAST_ERROR_MESSAGE = issueManager.GetLastErrorMessage();
			if (LAST_ERROR_MESSAGE == L"ゲーム終了")
			{
				break;  // 安全に終了
			}

			int a = MessageBox(
				nullptr,
				std::format(L"{}", LAST_ERROR_MESSAGE).c_str(),
				VCU_WIDE("ゲームが異常終了しました！"),
				MB_OK | MB_ICONERROR);
			break;  // エラーを表示して終了
		}

		// ウィンドウメッセージを確認
		if (!winMessage.Update())
		{
			break;
		}

		if (!gameTime.Update())
		{
			Sleep(0);
			continue;  // フレーム更新がされなかったなら回帰
		}

		callableSceneManager.Refresh();
#pragma region 更新処理
		// 生マウス入力データ更新
		rawMouseInput.Update();

		// ボタンの更新
		virtualInputManager.Update();

		// ImGuiのフレーム開始
		imguiManager.NewFrame();

		// 各オブジェクトの更新処理
		callableSceneManager.Update();

		// 各オブジェクトの遅い更新処理
		callableSceneManager.LateUpdate();

		// ライトの更新
		lightManager.Update();
#pragma endregion

#pragma region シャドウマップ描画
		// シャドウマップをクリアする
		dx11Manager.ClearAndBindShadowMap();

		// ライトの view proj を使えるようにする
		lightManager.Bind();

		// 各オブジェクトにシャドウマップ作ってもらう
		callableSceneManager.Draw(true);
		
		// シャドウマップの描画を終了していつもの描画へ変更
		dx11Manager.ResetViewport(clientConfig_.clientSize);
		dx11Manager.ResetRenderTargets();
#pragma endregion

#pragma region いつもの描画
		// 出来上がったシャドウマップを使えるようにする
		dx11Manager.BindShadowMap();
		
		// ライトの view proj を使えるようにする
		lightManager.Bind();
		
		// いつも通りの描画バッファクリア
		dx11Manager.Clear();

		// いつも通りの描画
		callableSceneManager.Draw(false);

		dx11Manager.UnbindShadowMap();

#pragma endregion

#pragma region 半透明描画処理
		// 半透明の描画を一番最後に
		dx11Manager.FlushTransparentQueue();
#pragma endregion

#pragma region UI描画
		// UIは一番最後に描画
		// D2D の Begin/End は U IManager が各レンダラーのバックエンドに応じて管理する
		uiManager.Update();
#pragma endregion

#pragma region デバッグ用UI描画処理
		imguiManager.Render();
#pragma endregion

		// バックバッファとスワップする
		dx11Manager.Swap();

#ifdef USE_DISCORD_RPC
		// discordも更新
		discordManager.Update();
#endif
	}

	sceneManager.Clear();

	ManagerRegistry::AllRelease();

	// リソース解放
	Resrc::Instance().Clear();

	CoUninitialize();
}
