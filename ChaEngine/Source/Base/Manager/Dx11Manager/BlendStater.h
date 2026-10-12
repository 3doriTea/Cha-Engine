#pragma once
#include "BlendStateType.h"

namespace cha
{
	/// @brief ブレンドステートをまとめたクラス
	class BlendStater
	{
	public:
		BlendStater() = default;
		~BlendStater() = default;

		/// @brief 初期化処理
		/// @param _pDevice デバイスのcomptr
		/// @return 初期化に成功 true / false
		bool Initialize(const ComPtr<ID3D11Device>& _pDevice);

		/// @brief ブレンドステートをセットする
		/// @param _type ブレンドステートの種類
		/// @param _pContext デバイスコンテキストのcomptr
		void Set(const BlendStateType _type, const ComPtr<ID3D11DeviceContext>& _pContext);

	private:
		std::array<ComPtr<ID3D11BlendState>, BlendStateType_Max> pBlendState_;  ///< ブレンドステート
	};
}
