#pragma once
#include <Base/Base.h>
#include <Base/Manager/Dx11Manager.h>

/// @brief 名前コンポーネント
struct Name : ComponentBase<Name>
{
	/// @brief 名前をセットする
	/// @param _content 名前のコンテンツ
	inline void SetName(const std::string_view _content) { content = _content; }

	/// @brief 名前を取得する
	/// @return 名前
	inline std::string_view GetName() const { return content; }
	
	/// @brief 名前コンポーネントの初期化処理
	constexpr Name() = default;

	/// @brief 名前コンポーネントの初期化処理
	/// @param _content 名前の初期値
	constexpr Name(const std::string_view _content) : 
		content{ _content }
	{}

	/// @brief stirng_viewへのキャスト演算子オーバーロード
	operator std::string_view() const
	{
		return content;
	}

	std::string content{};  ///< 名前本体
};
