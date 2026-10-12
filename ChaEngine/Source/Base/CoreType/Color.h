#pragma once
#include <DirectXMath.h>

namespace cha
{
	/// @brief 色構造体
	struct Color
	{
		enum
		{
			AT_R,    ///< 赤成分
			AT_G,    ///< 緑成分
			AT_B,    ///< 青成分
			AT_A,    ///< 不透明度
			AT_MAX,  ///< 色構造体パラメータ数
		};

		/// @brief 色構造体を各成分 0.0f ~ 1.0f で初期化
		/// @param _r 赤成分 0.0f ~ 1.0f
		/// @param _g 緑成分 0.0f ~ 1.0f
		/// @param _b 青成分 0.0f ~ 1.0f
		/// @param _a 不透明度成分 0.0f ~ 1.0f
		Color(
			const float _r,
			const float _g,
			const float _b,
			const float _a = 1.0f) :
			r{ _r },
			g{ _g },
			b{ _b },
			a{ _a }
		{}

		/// @brief 色構造体デフォルトコンストラクタ
		Color() :
			Color{ 1.0f, 1.0f, 1.0f }
		{}

#pragma region データ本体
		union
		{
			struct
			{
				float r;  ///< 赤色成分
				float g;  ///< 緑色成分
				float b;  ///< 青色成分
				float a;  ///< アルファ
			};
			DirectX::XMFLOAT4 xm;  ///< XMFLOAT4
			float f[AT_MAX];       ///< 配列としてアクセスする
		};
#pragma endregion

		/// @brief 色構造体を直接UINT型に変換
		inline operator UINT() const
		{
			UINT code{};

			code |= static_cast<UINT>(std::clamp(r, 0.0f, 1.0f) * UINT8_MAX) << (8 * 0);
			code |= static_cast<UINT>(std::clamp(g, 0.0f, 1.0f) * UINT8_MAX) << (8 * 1);
			code |= static_cast<UINT>(std::clamp(b, 0.0f, 1.0f) * UINT8_MAX) << (8 * 2);
			code |= static_cast<UINT>(std::clamp(a, 0.0f, 1.0f) * UINT8_MAX) << (8 * 3);

			return code;
		}
		
		/// @brief 色構造体をカラーコードから初期化する
		/// @param _code カラーコード
		inline Color(const UINT _code)
		{
			r = static_cast<float>((_code >> (8 * 0)) & UINT8_MAX) / static_cast<float>(UINT8_MAX);
			g = static_cast<float>((_code >> (8 * 1)) & UINT8_MAX) / static_cast<float>(UINT8_MAX);
			b = static_cast<float>((_code >> (8 * 2)) & UINT8_MAX) / static_cast<float>(UINT8_MAX);
			a = static_cast<float>((_code >> (8 * 3)) & UINT8_MAX) / static_cast<float>(UINT8_MAX);
		}

		/// @brief 色構造体を不透明度付きのWebカラーコードへ変換する
		/// @return 不透明度付きWebカラーコード
		inline UINT ToWebCodeA() const
		{
			UINT code{};
			code |= static_cast<UINT>(std::clamp(r, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 24; // Rを最上位に
			code |= static_cast<UINT>(std::clamp(g, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 16;
			code |= static_cast<UINT>(std::clamp(b, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 8;
			code |= static_cast<UINT>(std::clamp(a, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 0;  // Aを最下位に
			return code;
		}

		/// @brief 色構造体をWebカラーコードへ変換する
		/// @return Webカラーコード
		inline UINT ToWebCode() const
		{
			UINT code{};
			code |= static_cast<UINT>(std::clamp(r, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 16; // Rを最上位に
			code |= static_cast<UINT>(std::clamp(g, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 8;
			code |= static_cast<UINT>(std::clamp(b, 0.0f, 1.0f) * static_cast<float>(UINT8_MAX)) << 0;
			return code;
		}

		/// @brief α値を含むWebのカラーコードから色を取得
		/// @param _code Webカラーコード
		/// @return 色
		static inline Color FromWebCodeA(const UINT _code)
		{
			return Color
			{
				static_cast<float>((_code >> 24) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
				static_cast<float>((_code >> 16) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
				static_cast<float>((_code >> 8) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
				static_cast<float>((_code >> 0) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
			};
		}

		/// @brief α値を含むWebのカラーコードから色を取得
		/// @param _code Webカラーコード
		/// @return 色
		static inline Color FromWebCode(const UINT _code)
		{
			return Color
			{
				static_cast<float>((_code >> 16) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
				static_cast<float>((_code >> 8) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
				static_cast<float>((_code >> 0) & UINT8_MAX) / static_cast<float>(UINT8_MAX),
			};
		}

		static const Color WHITE;  ///< テンプレート色: 白
		static const Color BLACK;  ///< テンプレート色: 黒
	};

	inline const Color Color::WHITE{ 1, 1, 1 };
	inline const Color Color::BLACK{ 0, 0, 0 };
}
