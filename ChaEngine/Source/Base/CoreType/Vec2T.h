#pragma once
#include <cmath>

namespace cha
{
	/// @brief 2次元ベクトル
	/// @tparam T 32bitプリミティブ型
	/// @tparam DirectXMathT 対応するDirectXMathの型
	/// @tparam D2D1T 対応するDirect2Dの型
	/// @tparam BIGT 64bitプリミティブ型
	template<typename VecTraits>
	struct Vec2T
	{
		using T = typename VecTraits::T;
		using DirectXMathT = typename VecTraits::DirectXMathT;
		using D2D1T = typename VecTraits::D2D1T;
		using BIGT = typename VecTraits::BIGT;

		/// @brief 2次元ベクトルのPOD構造体
		struct Vec2TData
		{
			T x;  ///< x成分
			T y;  ///< y成分
		};

		enum : uint32_t
		{
			AT_X,    ///< x成分のインデックス
			AT_Y,    ///< y成分のインデックス
			AT_MAX,  ///< 2次元ベクトル成分の最大数
		};

		/// @brief デフォルトのデストラクタ
		inline ~Vec2T() = default;

		/// @brief 暗黙的なXMVECTORへのキャスト
		inline constexpr operator DirectX::XMVECTOR() const noexcept
		{
			// Tがfloat型なら
			if constexpr (std::is_same_v<T, float>)
			{
				return DirectX::XMLoadFloat2(reinterpret_cast<const DirectX::XMFLOAT2*>(this));
			}
			else
			{
				return { static_cast<float>(x), static_cast<float>(y) };
			}
		}

		/// @brief XMVECTORからの変換
		/// @param _xmv XMVECTOR
		inline constexpr Vec2T(const DirectX::XMVECTOR& _xmv) :
			Vec2T{ _xmv.m128_f32[AT_X], _xmv.m128_f32[AT_Y] }
		{
		}


		/// @brief POD構造体での初期化
		/// @param _in POD構造体
		inline constexpr Vec2T(Vec2TData _in) :
			Vec2T{ _in.x, _in.y }
		{
		}

		/// @brief デフォルトコンストラクタ
		inline constexpr Vec2T() :
			Vec2T{ 0, 0 }
		{
		}

		/// @brief 配列から初期化
		/// @param _pV 配列
		inline constexpr Vec2T(const T* _pV) :
			Vec2T{ _pV[AT_X], _pV[AT_Y] }
		{
		}

		/// @brief 64bit型の配列から初期化
		/// @param _pV 64bit型の配列
		inline Vec2T(const BIGT* _pV) :
			Vec2T
			{
				static_cast<T>(_pV[AT_X]),
				static_cast<T>(_pV[AT_Y])
			}
		{
		}
		
		/// @brief 他の型からの各成分初期化
		/// @tparam U 他の型
		/// @param _x x成分
		/// @param _y y成分
		template<typename U>
		inline Vec2T(U _x, U _y) :
			x{ static_cast<T>(_x) },
			y{ static_cast<T>(_y) }
		{
		}

#pragma region 同じ型からのコピーとムーブ
		/// @brief コピーコンストラクタを明示的に復活させる
		inline Vec2T(const Vec2T& _other) = default;
		/// @brief ムーブコンストラクタも明示的に復活させておく（念のため安全用）
		inline Vec2T(Vec2T&& _other) noexcept = default;

		/// @brief コピー代入演算子
		/// @param _other 同じ型
		/// @return 自身の参照
		inline Vec2T& operator=(const Vec2T& _other) = default;
		/// @brief ムーブ代入演算子
		/// @param _other 同じ型
		/// @return 自身の参照
		inline Vec2T& operator=(Vec2T&& _other) noexcept = default;
#pragma endregion

#pragma region 他の同系統
		/// @brief 他の同成分数からのコピーコンストラクタ
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		template<typename VecTraitsOther>
		inline constexpr Vec2T(const Vec2T<VecTraitsOther>& _other) :
			Vec2T{ _other.x, _other.y }
		{
		}

		/// @brief 他の同成分数からのコピー
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		/// @return コピーされる型
		template<typename VecTraitsOther>
		inline constexpr Vec2T& operator=(const Vec2T<VecTraitsOther>& _other)
		{
			this->x = static_cast<T>(_other.x);
			this->y = static_cast<T>(_other.y);
			return *this;
		}

		/// @brief 他の同成分数からのムーブコンストラクタ
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		template<typename VecTraitsOther>
		inline constexpr Vec2T(Vec2T<VecTraitsOther>&& _other) :
			Vec2T{ _other.x, _other.y }
		{
			_other.x = 0;
			_other.y = 0;
		}

		/// @brief 他の同成分数からのコピー
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		/// @return コピーされる型
		template<typename VecTraitsOther>
		inline constexpr Vec2T& operator=(Vec2T<VecTraitsOther>&& _other)
		{
			this->x = static_cast<T>(_other.x);
			this->y = static_cast<T>(_other.y);
		
			_other.x = 0;
			_other.y = 0;
		
			return *this;
		}
#pragma endregion

#pragma region データ本体
		union
		{
			struct
			{
				T x;  ///< x成分
				T y;  ///< y成分
			};
			T v[AT_MAX];      ///< 配列としてのアクセス
			DirectXMathT xm;  ///< DirectXMathの形としてアクセス
			D2D1T d2dv;       ///< Direct2D用のVECTOR2型
		};
#pragma endregion

#pragma region 便利系
		/// <summary>
		/// ベクトルの長さを求める
		/// </summary>
		/// <returns>長さ float</returns>
		float Length() const
		{
			return std::sqrtf(
				static_cast<float>(x) * static_cast<float>(x)
				+ static_cast<float>(y) * static_cast<float>(y)
			);
		};

		/// <summary>
		/// 正規化したベクトルを求める
		/// </summary>
		/// <returns>長さが1のベクトル</returns>
		Vec2T Normalize() const
		{
			float length{ Length() };
			if (length > 0.0f)
			{
				return *this / length;
			}
			else
			{
				return Vec2T{};
			}
		}
#pragma endregion

#pragma region 四則演算
		template<typename VecTraitsOther>
		inline Vec2T& operator+=(const Vec2T<VecTraitsOther> _other)
		{
			this->x += static_cast<T>(_other.x);
			this->y += static_cast<T>(_other.y);
			return *this;
		}
		template<typename VecTraitsOther>
		inline Vec2T& operator-=(const Vec2T<VecTraitsOther> _other)
		{
			this->x -= static_cast<T>(_other.x);
			this->y -= static_cast<T>(_other.y);
			return *this;
		}
		template<typename U>
		inline Vec2T& operator*=(const U _scalar)
		{
			this->x *= static_cast<T>(_scalar);
			this->y *= static_cast<T>(_scalar);
			return *this;
		}
		template<typename U>
		inline Vec2T& operator/=(const U _scalar)
		{
			this->x /= static_cast<T>(_scalar);
			this->y /= static_cast<T>(_scalar);
			return *this;
		}
		Vec2T operator-() const { return Vec2T{ -x, -y }; }
#pragma endregion

#pragma region 定数
		inline static Vec2T Zero()  { return Vec2T{ T{  0 }, T{  0 } }; }
		inline static Vec2T One()   { return Vec2T{ T{  1 }, T{  1 } }; }
		inline static Vec2T Right() { return Vec2T{ T{  1 }, T{  0 } }; }
		inline static Vec2T Left()  { return Vec2T{ T{ -1 }, T{  0 } }; }
		inline static Vec2T Up()    { return Vec2T{ T{  0 }, T{  1 } }; }
		inline static Vec2T Down()  { return Vec2T{ T{  0 }, T{ -1 } }; }
#pragma endregion
	};

#pragma region フレンド四則演算関数
	template<typename VecTraits>
	inline Vec2T<VecTraits> operator+(Vec2T<VecTraits> _left, const Vec2T<VecTraits> _right)
	{
		return _left += _right;
	}
	template<typename VecTraits>
	inline Vec2T<VecTraits> operator-(Vec2T<VecTraits> _left, const Vec2T<VecTraits> _right)
	{
		return _left -= _right;
	}
	template<typename VecTraits>
	inline Vec2T<VecTraits> operator*(Vec2T<VecTraits> _left, const typename VecTraits::T _right)
	{
		return _left *= _right;
	}
	template<typename VecTraits>
	inline Vec2T<VecTraits> operator*(const typename VecTraits::T _left, const Vec2T<VecTraits> _right)
	{
		return _right *= _left;
	}
	template<typename VecTraits>
	inline Vec2T<VecTraits> operator/(Vec2T<VecTraits> _left, const typename VecTraits::T _right)
	{
		return _left /= _right;
	}
	template<typename VecTraits>
	inline Vec2T<VecTraits> operator/(const typename VecTraits::T _left, const Vec2T<VecTraits> _right)
	{
		return _right /= _left;
	}
#pragma endregion

#pragma region フレンド比較演算子
	template<typename VecTraits>
	inline bool operator==(const Vec2T<VecTraits> _a, const Vec2T<VecTraits> _b)
	{
		return _a.x == _b.x && _a.y == _b.y;
	}
	template<typename VecTraits>
	inline bool operator!=(const Vec2T<VecTraits> _a, const Vec2T<VecTraits> _b)
	{
		return !(_a == _b);
	}
#pragma endregion
}
