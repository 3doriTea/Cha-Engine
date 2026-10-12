#pragma once
#include "../BasePch.h"

namespace cha::Mathf
{
	/// @brief 2点間を線形補間をする
	/// @tparam T 同じ型
	/// @param _a 開始値
	/// @param _b 終了値
	/// @param _t 補間値
	/// @return _a から _b を _t 値で線形補間
	template<typename T>
	inline static T Lerp(const T _a, const T _b, const float _t)
	{
		float t = std::clamp(_t, 0.0f, 1.0f);

		// 正しい線形補間の式 (掛け算)
		return _a + (_b - _a) * t;
	}

	/// @brief 度数法を弧度法に変換する
	/// @param _degree 角度(度数法)
	/// @return 角度(弧度法)
	inline static float ToRad(const float _degree)
	{
		return (DirectX::XM_PI / 180.0f) * _degree;
	}

	/// @brief 弧度法を度数法に変換する
	/// @param _radian 角度(弧度法)
	/// @return 角度(度数法)
	inline static float ToDeg(const float _radian)
	{
		return (180.0f / DirectX::XM_PI) * _radian;
	}

	/// @brief 行列からスケール成分を除去する
	/// @param _matrix スケールを1にしたい行列
	/// @return スケールが1の行列
	inline Mat4x4 RemoveScale(const Mat4x4& _matrix)
	{
		DirectX::XMVECTOR scale{};
		DirectX::XMVECTOR rotation{};
		DirectX::XMVECTOR translation{};

		// 行列を スケール、回転クォータニオン、平行移動 に分解する
		DirectX::XMMatrixDecompose(&scale, &rotation, &translation, _matrix);

		// スケール成分を完全に無視して、回転と平行移動だけで行列を再構築する
		// （スケールに単位元である Vector1 (1.0f, 1.0f, 1.0f, 1.0f) を指定する）
		DirectX::XMMATRIX pureRotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(translation);

		return XMMatrixMultiply(pureRotationMatrix, translationMatrix);
	}
}
