#pragma once
#include <DirectXMath.h>
#include "Vec3.h"
#include "Vec2.h"
#include "CoreTypeMath.h"

namespace cha
{
	/// @brief プリミティブ数値型 4つ分
	/// @tparam T プリミティブ数値型
	/// @tparam DirectXMathT 対応するDirectXMathの型
	template<typename T, typename DirectXMathT>
	struct Type4
	{
		enum
		{
			AT_X,    ///< X成分
			AT_Y,    ///< Y成分
			AT_Z,    ///< Z成分
			AT_W,    ///< W成分
			AT_MAX,  ///< Type4の成分最大数
		};

		/// @brief デフォルトコンストラクタ
		inline constexpr Type4() :
			Type4{ 0.0f, 0.0f, 0.0f, 1.0f }
		{}

		/// @brief DirectXの型からコピーして初期化する
		/// @param _xm DirectXの型の値
		inline Type4(const DirectXMathT& _xm) :
			xm{ _xm }
		{}

		/// @brief DirectXの型からムーブしてくる
		/// @param _xm DirectXの型の値
		inline Type4(DirectXMathT&& _xm) :
			xm{ std::move(_xm) }
		{}

		/// @brief XMVECTORからコピーして初期化する
		/// @param _xmv XMVECTORの型の値
		inline Type4(const DirectX::XMVECTOR& _xmv)
			requires std::is_same_v<T, float>
		{
			DirectX::XMStoreFloat4(&xm, _xmv);
		}

		/// @brief Vec3からコピーして初期化する
		/// @param _v3 Vec3の型の値
		inline Type4(const Vec3& _v3) :
			Type4{ _v3.x, _v3.y, _v3.z, 1.0f }
		{
		}
		
		/// @brief Vec2からコピーして初期化する
		/// @param _v2 Vec2の型の値
		inline Type4(const Vec2& _v2) :
			Type4{ _v2.x, _v2.y, 0.0f, 1.0f }
		{
		}

		/// @brief 各成分を初期化する
		/// @param _x x成分
		/// @param _y y成分
		/// @param _z z成分
		/// @param _w w成分
		inline Type4(const T _x, const T _y, const T _z, const T _w = 1) :
			x{ _x },
			y{ _y },
			z{ _z },
			w{ _w }
		{}

		/// @brief 2つのType4を線形補間する
		/// @param _a Type4の始点
		/// @param _b Type4の終点
		/// @param _ratio 補間の割合
		/// @return 線形補間されたType4
		inline static Type4 Lerp(const Type4 _a, const Type4 _b, const float _ratio)
		{
			return Type4
			{
				CoreTypeMath::Lerp(_a.x, _b.x, _ratio),
				CoreTypeMath::Lerp(_a.y, _b.y, _ratio),
				CoreTypeMath::Lerp(_a.z, _b.z, _ratio),
				CoreTypeMath::Lerp(_a.w, _b.w, _ratio),
			};
		}

		/// @brief XMVECTORへ暗黙的キャストする
		inline operator DirectX::XMVECTOR() const noexcept
		{
			if constexpr (std::is_same_v<T, float>)
			{
				return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(this));
			}
			else
			{
				return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
			}
		}

#pragma region データ本体
		union
		{
			T v[AT_MAX];      ///< 配列としてアクセス
			DirectXMathT xm;  ///< DirectXの値としてアクセス
			struct
			{
				T x;  ///< x成分
				T y;  ///< y成分
				T z;  ///< z成分
				T w;  ///< w成分
			};
		};
#pragma endregion
	};
}
