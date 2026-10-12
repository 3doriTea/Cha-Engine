#pragma once
#include <Base/Base.h>

struct UILayerOrderData
{
	union
	{
		uint64_t raw;  ///< 生データ
		struct
		{
			uint64_t padding : 28;  ///< 未使用
			uint64_t user : 32;     ///< ユーザブルなレイヤー（最優先）
			uint64_t uiType : 2;    ///< UIの種別（図形か、文字か）
			uint64_t funcType : 2;  ///< 処理の種類（前処理か、描画か）
		} bits;  ///< 各ビットでアクセス
	};
};

/// @brief UIのレイヤーオーダー 値が高いほど「「手前」」に描画される
struct UILayerOrder : UILayerOrderData
{
	using FuncType = uint8_t;
	enum FuncType_ : FuncType
	{
		FuncType_Preparation = 0,  ///< レンダリングの前の準備処理
		FuncType_Rendering = 1,    ///< 実際の描画
	};

	using UIType = uint8_t;
	enum UIType_ : UIType
	{
		UIType_Figure = 0,  ///< 図形（奥に描画）
		UIType_Text = 1,    ///< テキスト（手前に描画）
	};

	/// @brief UIレイヤーオーダーの初期化処理
	constexpr UILayerOrder() :
		UILayerOrder{ 0 }
	{}

	/// @brief UIレイヤーオーダーの初期化処理
	constexpr UILayerOrder(const uint32_t _userLayer) :
		UILayerOrderData
		{
			.bits
			{
				.padding = 0,
				.user = _userLayer,
				.uiType = 0,
				.funcType = FuncType_Rendering,
			}
		}
	{
	}

	/// @brief 宇宙船演算子のオーバーロード
	/// @param _other 
	/// @return 
	inline auto operator<=>(const UILayerOrder& _other) const
	{
		// ビットフィールドの配置は実装依存であるため、
		// raw 値の比較に依存せず、意図した優先度（user -> uiType -> funcType -> padding）で比較する
		auto lhs = std::tuple<uint32_t, uint8_t, uint8_t, uint32_t>{
			static_cast<uint32_t>(bits.user),
			static_cast<uint8_t>(bits.uiType),
			static_cast<uint8_t>(bits.funcType),
			static_cast<uint32_t>(bits.padding)
		};
		auto rhs = std::tuple<uint32_t, uint8_t, uint8_t, uint32_t>{
			static_cast<uint32_t>(_other.bits.user),
			static_cast<uint8_t>(_other.bits.uiType),
			static_cast<uint8_t>(_other.bits.funcType),
			static_cast<uint32_t>(_other.bits.padding)
		};
		return lhs <=> rhs;
	}

	/// @brief 同値判定演算子のオーバーロード
	/// @param _other 
	/// @return 
	inline bool operator==(const UILayerOrder& _other) const
	{
		return raw == _other.raw;
	}

	/// @brief 指定したユーザーレイヤーに変更したオーダーを返す
	[[nodiscard]] constexpr UILayerOrder WithUser(uint32_t _userLayer) const
	{
		UILayerOrder copy = *this;
		copy.bits.user = _userLayer;
		return copy;
	}

	/// @brief 指定したUI種別に変更したオーダーを返す
	[[nodiscard]] constexpr UILayerOrder WithUIType(UIType _uiType) const
	{
		UILayerOrder copy = *this;
		copy.bits.uiType = _uiType;
		return copy;
	}

	/// @brief 指定した処理の種類に変更したオーダーを返す
	[[nodiscard]] constexpr UILayerOrder WithFuncType(FuncType _funcType) const
	{
		UILayerOrder copy = *this;
		copy.bits.funcType = _funcType;
		return copy;
	}
};
