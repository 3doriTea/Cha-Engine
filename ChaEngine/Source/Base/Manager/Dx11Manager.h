#pragma once
#include "../BasePch.h"
#include "IManager.h"
#include "../Core/Resource/ResourceType.h"
#include "Dx11Manager/DepthState.h"
#include "Dx11Manager/DepthStateType.h"
#include "Dx11Manager/BlendStater.h"
#include "../Core/Resource.h"
#include "../Core/Resource/Texture.h"

namespace cha
{
	template<ResourceType ResourceTypeV>
	class Resource;
	struct SurfaceResource;

	/// @brief DirectX11 をざらっと扱う
	class Dx11Manager : public IManager
	{
	public:
		Dx11Manager(
			const HWND _hWnd,
			const Vec2Int _clientSize,
			const Vec2 _shadowMapSize,
			const bool _isWindow);

		bool Initialize();

		/// @brief 普通の描画バックバッファをクリアする
		void Clear();
		/// @brief 普通の描画画面をスワップする
		void Swap();

		/// @brief シャドウマップのクリアとシャドウマップ用にバインド
		void ClearAndBindShadowMap();
		/// @brief シャドウマップのバインドを解除する
		void UnbindShadowMap();

		/// @brief 出来上がったシャドウマップを影描画用に参照可能にする
		void BindShadowMap();

		/// @brief サーフェースをセットする
		/// @param _resource サーフェス
		void SetSurface(const Resource<ResourceType::Surface>& _resource);
		/// @brief サーフェスをセットする
		/// @param _meta サーフェース
		void SetSurface(const SurfaceResource& _meta);

		/// @brief シェーダをセットする
		/// @param _resource シェーダリソース
		void SetShader(const Resource<ResourceType::Shader>& _resource);

		/// @brief シェーダProをセットする
		/// @param _resource シェーダProリソース
		void SetShader(const Resource<ResourceType::ShaderPro>& _resource);


		/// @brief テクスチャをセットする
		/// @param _resource テクスチャリソース
		/// @param _slot スロット番号
		void SetTexture(const Resource<ResourceType::Texture>& _resource, const UINT _slot = 0);

		/// @brief マテリアルをセットする
		/// @param _resource マテリアルリソース
		void SetMaterial(const Resource<ResourceType::Material>& _resource);

		/// @brief 描画する
		void Draw() const;

		/// @brief 透明描画を後回しにするためのキュー登録
		void EnqueueTransparent(std::function<void()> _fn);

		/// @brief 透明描画を後回しにするためのキュー登録 frontに
		void EnqueueTransparentFront(std::function<void()> _fn);
		/// @brief キューに溜めた透明描画を実行・クリア
		void FlushTransparentQueue();

		/// @brief インデクスを指定して描画する
		/// @param _indexCount インデクス数
		void Draw(const UINT _indexCount) const;

		/// @brief 頂点数を指定して頂点描画する
		/// @param _vertexCount 頂点数
		void DrawVertex(const UINT _vertexCount) const;

		/// @brief ID3D11Device / ID3D11DeviceContext を参照する
		/// @tparam Func Any(const ComPtr<ID3D11Device>&, const ComPtr<ID3D11DeviceContext>&)
		/// @param _func 任意の戻り値を返すラムダ式
		/// @return Any(任意の型)
		template<typename Func>
		inline decltype(auto) Ref(Func&& _func)
		{
			if constexpr (std::is_invocable_v<Func, decltype(pDevice_), decltype(pContext_)>)
			{
				return _func(pDevice_, pContext_);
			}
			else if constexpr (std::is_invocable_v<Func, decltype(pContext_), decltype(pDevice_)>)
			{
				return _func(pContext_, pDevice_);
			}
			else if constexpr (std::is_invocable_v<Func, decltype(pDevice_)>)
			{
				return _func(pDevice_);
			}
			else if constexpr (std::is_invocable_v<Func, decltype(pContext_)>)
			{
				return _func(pContext_);
			}
			else
			{
				static_assert(
					dependent_false<Func>,
					"ComPtr<D3D11Device>かComPtr<D3D11DeviceContext>またはその両方を受け取る引数を用意してください。");
			}
		}

		/// @brief スワップチェーンを参照する
		/// @tparam Func Any(const ComPtr<ID3D11Device>&, const ComPtr<ID3D11DeviceContext>&)
		/// @param _func 任意の戻り値を返すラムダ式
		/// @return Any(任意の型)
		template<typename Func>
		inline decltype(auto) RefSwapChain(Func&& _func)
		{
			return _func(pSwapChain_);
		}

		/// @brief 深度バッファステートをセットする
		/// @param _depthStateType 深度バッファステート
		void ChangeDepthState(const DepthStateType _depthStateType);

		// TODO: debug用
#ifdef _DEBUG
		inline DepthState& RefDepthState(const DepthStateType _depthStateType)
		{
			return depthState_[_depthStateType];
		}
#endif

		/// @brief ビューポートをリセットする 解像度変更とかできる
		/// @return リセットに成功したか
		bool ResetViewport(const Vec2 _size);

		/// @brief レンダーターゲットと深度バッファをセットする
		bool ResetRenderTargets();

		/// @brief ブレンドステートをセットする
		/// @param _type ブレンドステートの種類
		void SetBlendState(const BlendStateType _type);

	private:
		/// @brief スワップチェーンを生成
		/// @param _factory IDXGIFactory
		/// @return 成功した true / false
		bool GenerateSwapChain(ComPtr<IDXGIFactory>& _factory);
		/// @brief 裏バッファのレンダーターゲットビューを生成
		/// @param _swapChainDesc 
		/// @return 成功した true / false
		bool GenerateBackBufferRTV(const DXGI_SWAP_CHAIN_DESC& _swapChainDesc);

		/// @brief 震度バッファを生成する
		/// @param _desc 深度バッファステンシルの設定
		/// @return 作成に成功 true / false
		bool GenerateDepthState(const D3D11_DEPTH_STENCIL_DESC& _desc);

		/// @brief 深度バッファを生成する
		/// @param _size 
		/// @return 
		bool GenerateDepthBuffer(const Vec2 _size);

		/// @brief シャドウマップを生成する
		/// @param _shadowMapWidth シャドウマップの横幅
		/// @param _shadowMapHeight シャドウマップの高さ
		/// @return 生成に成功 true / falae
		bool GenerateShadowMap(const UINT _shadowMapWidth, const UINT _shadowMapHeight);

	private:
		const HWND WND_HANDLE_;
		const Vec2Int CLIENT_SIZE_;
		const Vec2 SHADOW_MAP_SIZE_;
		const bool IS_WINDOW_;
		BlendStater blendStater{};                         ///< ブレンドステート
		UINT drawIndexCount_{};                            ///< 描画するインデックス数
		UINT drawVertexCount_{};                           ///< 描画する頂点数
		D3D11_VIEWPORT currentViewport_{};                 ///< 現在のビューポートの設定
		Color fillColor_{ Color::BLACK };                  ///< 塗りつぶす色
		ComPtr<ID3D11Device> pDevice_{};                   ///< デバイス
		ComPtr<ID3D11DeviceContext> pContext_{};           ///< デバイスコンテキスト
		ComPtr<IDXGISwapChain> pSwapChain_{};              ///< スワップチェーン
		ComPtr<ID3D11RenderTargetView> pBackBufferRTV_{};  ///< 裏バッファのレンダーターゲットビュー

		ComPtr<ID3D11Texture2D> pDepthStencilBuffer_{};  ///< 深度バッファ

		std::array<DepthState, DepthStateType_Max> depthState_{};  ///< 深度バッファの書き込み状態
		DepthStateType currentDepthStateType_{};  ///< 現在の深度バッファの種類

		ComPtr<ID3D11Texture2D> pShadowTexture2D_{};   ///< シャドウマップ用テクスチャ
		ComPtr<ID3D11DepthStencilView> pShadowDSV_{};  ///< シャドウマップ用のデプスステンシルビュー

		std::unique_ptr<Resource<ResourceType::Texture>> pShadowTexture_{};  ///< シャドウマップのテクスチャ
	};
}
