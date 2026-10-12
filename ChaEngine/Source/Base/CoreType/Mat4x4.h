#pragma once
#include <DirectXMath.h>

namespace cha
{
	/// @brief 4x4行列構造体
	struct Mat4x4 : DirectX::XMMATRIX
	{
		/// @brief XMMATRIXのコンストラクタを引き継ぐ
		using DirectX::XMMATRIX::XMMATRIX;

		/// @brief XMMATRIXをムーブして受け取る
		/// @param _other XMMATRIX
		inline Mat4x4(DirectX::XMMATRIX&& _other) :
			DirectX::XMMATRIX{ std::move(_other) }
		{}

		/// @brief XMMATRIXをコピーして受け取る
		/// @param _other XMMATRIX
		inline Mat4x4(const DirectX::XMMATRIX& _other) :
			DirectX::XMMATRIX{ _other }
		{}

		/// @brief デフォルトコンストラクタ 単位行列になる
		inline Mat4x4() :
			DirectX::XMMATRIX{ DirectX::XMMatrixIdentity() }
		{}

		/// @brief 行列の各要素を個別に指定する
		/// @note 引数は double 型ですが、内部で float 型にキャストされて DirectX::XMMATRIX に格納されます。
		/// @param m00 1行1列目の成分
		/// @param m01 1行2列目の成分
		/// @param m02 1行3列目の成分
		/// @param m03 1行4列目の成分
		/// @param m10 2行1列目の成分
		/// @param m11 2行2列目の成分
		/// @param m12 2行3列目の成分
		/// @param m13 2行4列目の成分
		/// @param m20 3行1列目の成分
		/// @param m21 3行2列目の成分
		/// @param m22 3行3列目の成分
		/// @param m23 3行4列目の成分
		/// @param m30 4行1列目の成分
		/// @param m31 4行2列目の成分
		/// @param m32 4行3列目の成分
		/// @param m33 4行4列目の成分
		inline Mat4x4(
			double m00, double m01, double m02, double m03,
			double m10, double m11, double m12, double m13,
			double m20, double m21, double m22, double m23,
			double m30, double m31, double m32, double m33) noexcept :
			DirectX::XMMATRIX
			{
				static_cast<float>(m00), static_cast<float>(m01), static_cast<float>(m02), static_cast<float>(m03),
				static_cast<float>(m10), static_cast<float>(m11), static_cast<float>(m12), static_cast<float>(m13),
				static_cast<float>(m20), static_cast<float>(m21), static_cast<float>(m22), static_cast<float>(m23),
				static_cast<float>(m30), static_cast<float>(m31), static_cast<float>(m32), static_cast<float>(m33),
			}
		{}

		/// @brief 行列の各行から初期化する
		/// @param row1 1行目の成分
		/// @param row2 2行目の成分
		/// @param row3 3行目の成分
		/// @param row4 4行目の成分
		inline Mat4x4(
			const DirectX::XMVECTOR& row1,
			const DirectX::XMVECTOR& row2,
			const DirectX::XMVECTOR& row3,
			const DirectX::XMVECTOR& row4) :
			DirectX::XMMATRIX
			{
				row1,
				row2,
				row3,
				row4,
			}
		{}

		/// @brief 単位行列を取得する
		/// @return 単位行列
		static inline Mat4x4 Identity() { return DirectX::XMMatrixIdentity(); }
	};
}
