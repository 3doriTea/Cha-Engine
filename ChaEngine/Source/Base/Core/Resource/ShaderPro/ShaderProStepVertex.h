#pragma once
#include "IShaderProStep.h"

namespace cha
{
	/// @brief シェーダプロステップ：頂点シェーダ
	class ShaderProStepVertex : public IShaderProStep
	{
	public:
		ShaderProStepVertex() = default;
		~ShaderProStepVertex() = default;

		/// @brief 試しにシェーダを読み込む
		/// @param _dx11 Dx11Managerの参照
		/// @param _file ファイルパス
		/// @param _pInclude インクルード設定
		/// @param _entryName エントリポイント名
		/// @param _version コンパイラバージョン
		/// @param _flags フラグ
		/// @return 読み込みに成功 true / false
		bool TryLoad(
			Dx11Manager& _dx11,
			const fs::path& _file,
			ID3DInclude* _pInclude,
			const std::string_view _entryName,
			const std::string_view _version,
			const UINT _flags) override;

		/// @brief シェーダをパイプラインにセットする
		/// @param _dx11 Dx11Managerの参照
		void Set(Dx11Manager& _dx11) override;


	private:
		ComPtr<ID3D11VertexShader> pVertexShader_{};  // 頂点シェーダ
		ComPtr<ID3D11InputLayout> pInputLayout_{};    // 頂点インプットレイアウト
	};
}
