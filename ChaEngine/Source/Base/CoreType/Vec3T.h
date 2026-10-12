#pragma once
#include <cmath>

namespace cha
{
	/// @brief 3次元ベクトル
	/// @tparam T 32bitプリミティブ型
	/// @tparam DirectXMathT 対応するDirectXMathの型
	/// @tparam BIGT 64bitプリミティブ型
	template<typename VecTraits>
	struct Vec3T
	{
		using T = typename VecTraits::T;
		using DirectXMathT = typename VecTraits::DirectXMathT;
		using D2D1T = typename VecTraits::D2D1T;
		using BIGT = typename VecTraits::BIGT;

		/// @brief 3次元ベクトルのPOD構造体
		struct Vec3TData
		{
			T x;  ///< x成分
			T y;  ///< y成分
			T z;  ///< z成分
		};

		enum
		{
			AT_X,    ///< x成分のインデックス
			AT_Y,    ///< y成分のインデックス
			AT_Z,    ///< z成分のインデックス
			AT_MAX,  ///< 3次元ベクトル成分の最大数
		};

		/// @brief デフォルトのデストラクタ
		inline ~Vec3T() = default;

		/// @brief 暗黙的なXMVECTORへのキャスト
		inline constexpr operator DirectX::XMVECTOR() const noexcept
		{
			// Tがfloat型なら
			if constexpr (std::is_same_v<T, float>)
			{
				return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(this));
			}
			else
			{
				return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
			}
		}

		/// @brief XMVECTORからの変換
		/// @param _xmv XMVECTOR
		inline constexpr Vec3T(const DirectX::XMVECTOR& _xmv) :
			Vec3T{ _xmv.m128_f32[AT_X], _xmv.m128_f32[AT_Y], _xmv.m128_f32[AT_Z] }
		{
		}

		/// @brief POD構造体での初期化
		/// @param _in POD構造体
		inline constexpr Vec3T(Vec3TData _in) :
			Vec3T{ _in.x, _in.y, _in.z }
		{
		}

		
		/// @brief デフォルトコンストラクタ
		inline constexpr Vec3T() :
			Vec3T{ 0, 0, 0 }
		{
		}
		
		/// @brief 配列から初期化
		/// @param _pV 配列
		inline constexpr Vec3T(const T* _pV) :
			Vec3T{ _pV[AT_X], _pV[AT_Y], _pV[AT_Z] }
		{
		}
		
		/// @brief 64bit型の配列から初期化
		/// @param _pV 64bit型の配列
		inline constexpr Vec3T(const BIGT* _pV) :
			Vec3T
			{
				static_cast<T>(_pV[AT_X]),
				static_cast<T>(_pV[AT_Y]),
				static_cast<T>(_pV[AT_Z])
			}
		{
		}

		/// @brief 他の型からの各成分初期化
		/// @tparam U 他の型
		/// @param _x x成分
		/// @param _y y成分
		/// @param _z z成分
		template<typename U>
		inline constexpr Vec3T(U _x, U _y, U _z) :
			x{ static_cast<T>(_x) },
			y{ static_cast<T>(_y) },
			z{ static_cast<T>(_z) }
		{
		}

#pragma region 同じ型からのコピーとムーブ
		/// @brief コピーコンストラクタを明示的に復活させる
		inline Vec3T(const Vec3T& _other) = default;
		/// @brief ムーブコンストラクタも明示的に復活させておく（念のため安全用）
		inline Vec3T(Vec3T&& _other) noexcept = default;

		/// @brief コピー代入演算子
		/// @param _other 同じ型
		/// @return 自身の参照
		inline Vec3T& operator=(const Vec3T& _other) = default;
		/// @brief ムーブ代入演算子
		/// @param _other 同じ型
		/// @return 自身の参照
		inline Vec3T& operator=(Vec3T&& _other) noexcept = default;
#pragma endregion

#pragma region 他の同系統
		/// @brief 他の同成分数からのコピーコンストラクタ
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		template<typename VecTraitsOther>
		inline constexpr Vec3T(const Vec3T<VecTraitsOther>& _other) :
			Vec3T{ _other.x, _other.y, _other.z }
		{
		}

		/// @brief 他の同成分数からのコピー
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		/// @return コピーされる型
		template<typename VecTraitsOther>
		inline constexpr Vec3T& operator=(const Vec3T<VecTraitsOther>& _other)
		{
			this->x = static_cast<T>(_other.x);
			this->y = static_cast<T>(_other.y);
			this->z = static_cast<T>(_other.z);
			return *this;
		}

		/// @brief 他の同成分数からのムーブコンストラクタ
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		template<typename VecTraitsOther>
		inline constexpr Vec3T(Vec3T<VecTraitsOther>&& _other) :
			Vec3T{ _other.x, _other.y, _other.z }
		{
			_other.x = 0;
			_other.y = 0;
			_other.z = 0;
		}

		/// @brief 他の同成分数からのコピー
		/// @tparam VecTraitsOther 他の同成分数の型
		/// @param _other 他の同成分数
		/// @return コピーされる型
		template<typename VecTraitsOther>
		inline constexpr Vec3T& operator=(Vec3T<VecTraitsOther>&& _other)
		{
			this->x = static_cast<T>(_other.x);
			this->y = static_cast<T>(_other.y);
			this->z = static_cast<T>(_other.z);

			_other.x = 0;
			_other.y = 0;
			_other.z = 0;

			return *this;
		}
#pragma endregion
		
#pragma region データ本体
		union
		{
			struct
			{
				T x;   ///< x成分
				T y;   ///< y成分
				T z;   ///< z成分
			};
			T v[AT_MAX];      ///< 配列としてアクセス
			DirectXMathT xm;  ///< DirectXMathの形としてアクセス
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
				+ static_cast<float>(z) * static_cast<float>(z)
			);
		};

		/// <summary>
		/// 正規化したベクトルを求める
		/// </summary>
		/// <returns>長さが1のベクトル</returns>
		Vec3T Normalize() const
		{
			float length{ Length() };
			if (length > 0.0f)
			{
				return *this / length;
			}
			else
			{
				return Vec3T{};
			}
		}
#pragma endregion

#pragma region 四則演算
		template<typename VecTraitsOther>
		inline Vec3T& operator+=(const Vec3T<VecTraitsOther> _other)
		{
			this->x += static_cast<T>(_other.x);
			this->y += static_cast<T>(_other.y);
			this->z += static_cast<T>(_other.z);
			return *this;
		}
		template<typename VecTraitsOther>
		inline Vec3T& operator-=(const Vec3T<VecTraitsOther> _other)
		{
			this->x -= static_cast<T>(_other.x);
			this->y -= static_cast<T>(_other.y);
			this->z -= static_cast<T>(_other.z);
			return *this;
		}
		template<typename U>
		inline Vec3T& operator*=(const U _scalar)
		{
			this->x *= static_cast<T>(_scalar);
			this->y *= static_cast<T>(_scalar);
			this->z *= static_cast<T>(_scalar);
			return *this;
		}
		template<typename U>
		inline Vec3T& operator/=(const U _scalar)
		{
			this->x /= static_cast<T>(_scalar);
			this->y /= static_cast<T>(_scalar);
			this->z /= static_cast<T>(_scalar);
			return *this;
		}
		Vec3T operator-() const { return Vec3T{ -x, -y, -z }; }
#pragma endregion

#pragma region 定数
		inline static Vec3T Zero()     { return Vec3T{ T{  0 }, T{  0 }, T{  0 } }; }
		inline static Vec3T One()      { return Vec3T{ T{  1 }, T{  1 }, T{  1 } }; }
		inline static Vec3T Right()    { return Vec3T{ T{  1 }, T{  0 }, T{  0 } }; }
		inline static Vec3T Left()     { return Vec3T{ T{ -1 }, T{  0 }, T{  0 } }; }
		inline static Vec3T Up()       { return Vec3T{ T{  0 }, T{  1 }, T{  0 } }; }
		inline static Vec3T Down()     { return Vec3T{ T{  0 }, T{ -1 }, T{  0 } }; }
		inline static Vec3T Forward()  { return Vec3T{ T{  0 }, T{  0 }, T{  1 } }; }
		inline static Vec3T Back()     { return Vec3T{ T{  0 }, T{  0 }, T{ -1 } }; }
#pragma endregion
	};

#pragma region フレンド四則演算関数
	template<typename VecTraits>
	inline Vec3T<VecTraits> operator+(Vec3T<VecTraits> _left, const Vec3T<VecTraits> _right)
	{
		return _left += _right;
	}
	template<typename VecTraits>
	inline Vec3T<VecTraits> operator-(Vec3T<VecTraits> _left, const Vec3T<VecTraits> _right)
	{
		return _left -= _right;
	}
	template<typename VecTraits>
	inline Vec3T<VecTraits> operator*(Vec3T<VecTraits> _left, const typename VecTraits::T _right)
	{
		return _left *= _right;
	}
	template<typename VecTraits>
	inline Vec3T<VecTraits> operator*(const typename VecTraits::T _left, const Vec3T<VecTraits> _right)
	{
		return _right *= _left;
	}
	template<typename VecTraits>
	inline Vec3T<VecTraits> operator/(Vec3T<VecTraits> _left, const typename VecTraits::T _right)
	{
		return _left /= _right;
	}
	template<typename VecTraits>
	inline Vec3T<VecTraits> operator/(const typename VecTraits::T _left, const Vec3T<VecTraits> _right)
	{
		return _right /= _left;
	}
#pragma endregion

#pragma region フレンド比較演算子
	template<typename VecTraits>
	inline bool operator==(const Vec3T<VecTraits> _a, const Vec3T<VecTraits> _b)
	{
		return _a.x == _b.x && _a.y == _b.y && _a.z == _b.z;
	}
	template<typename VecTraits>
	inline bool operator!=(const Vec3T<VecTraits> _a, const Vec3T<VecTraits> _b)
	{
		return !(_a == _b);
	}
#pragma endregion
}
