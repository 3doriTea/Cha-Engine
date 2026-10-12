#pragma once
#include "IShaderProStep.h"


namespace cha
{
	/// @brief シェーダプロステップ：ジオメトリシェーダ
	class ShaderProStepGeometry : public IShaderProStep
	{
	public:
		ShaderProStepGeometry() = default;
		~ShaderProStepGeometry() = default;

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


	//private:
		ComPtr<ID3D11GeometryShader> pGeometryShader_{};  // ジオメトリシェーダ
	};
}
