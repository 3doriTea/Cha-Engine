#pragma once
#include "../../../BasePch.h"

namespace cha
{
	class Dx11Manager;  // dx11Managerの前方宣言

	/// @brief シェーダのステップインタフェース
	class IShaderProStep
	{
	public:
		IShaderProStep() = default;
		virtual ~IShaderProStep() = default;

		/// @brief 試しにシェーダを読み込む
		/// @param _dx11 Dx11Managerの参照
		/// @param _file ファイルパス
		/// @param _pInclude インクルード設定
		/// @param _entryName エントリポイント名
		/// @param _version コンパイラバージョン
		/// @param _flags フラグ
		/// @return 読み込みに成功 true / false
		virtual bool TryLoad(
			Dx11Manager& _dx11,
			const fs::path& _file,
			ID3DInclude* _pInclude,
			const std::string_view _entryName,
			const std::string_view _version,
			const UINT _flags) = 0;

		/// @brief シェーダをパイプラインにセットする
		/// @param _dx11 Dx11Managerの参照
		virtual void Set(Dx11Manager& _dx11) = 0;
	};
}
