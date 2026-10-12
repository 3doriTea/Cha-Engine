#pragma once

/// マクロとしての mim max を使わないためのマクロ定義
#define NOMINMAX

#include <Windows.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include <d3d11.h>

#include <d2d1_1.h>
#include <dwrite_3.h>
#include <dwrite.h>

// DirectWriteを使用するために必須
#pragma comment(lib, "dwrite.lib")

// DirectWrite/Direct2Dを連携させるために通常必須
#pragma comment(lib, "d2d1.lib")

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <wrl/client.h>

#include <combaseapi.h>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#pragma comment(lib, "WinMM.lib")
#include <mmsystem.h>

// デバイス入力系
/// DirectInputのバージョンマクロ定義
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")

#include <ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// テクスチャ読み込み用
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

// 整数値型
#include <cstdint>

// 文字列
#include <string>
#include <string_view>

// 型情報
#include <typeindex>

// コレクション
#include <array>
#include <vector>
#include <set>
#include <map>
#include <queue>

// 参照だけのスパン
#include <span>

#include <bitset>

// ファイル読み込み
#include <fstream>

// 非同期関係
#include <atomic>
#include <thread>

// メモリ
#include <memory>

// 関数型
#include <functional>

// コンセプト
#include <concepts>

// fs
#include <filesystem>

#include <type_traits>

// 非同期関連
#include <thread>
#include <mutex>

namespace fs = std::filesystem;

#include "CoreType/FixedString.h"
#include "CoreType/Color.h"
#include "CoreType/Float4.h"
#include "CoreType/Float2.h"
#include "CoreType/Int4.h"
#include "CoreType/Handle.h"
#include "CoreType/Mat4x4.h"
#include "CoreType/Quaternion.h"
#include "CoreType/Type4T.h"
#include "CoreType/Vec2.h"
#include "CoreType/Vec2Int.h"
#include "CoreType/Vec2T.h"
#include "CoreType/Vec3.h"
#include "CoreType/Vec3Int.h"
#include "CoreType/Vec3T.h"
#include "CoreType/XMOperator.h"
#include "CoreType/ComponentFlag.h"

#include "Utility/HashCombine.h"

/// @brief ComPtr を省略
using Microsoft::WRL::ComPtr;

/// @brief static_assertをelse節で正しく動作させるためのヘルパー
/// @tparam ... 
template <class...> constexpr std::false_type dependent_false{};

#include <nlohmann-json/json.hpp>

/// @brief nlohmann::jsonの省略系 json!
using json = nlohmann::json;

// imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
