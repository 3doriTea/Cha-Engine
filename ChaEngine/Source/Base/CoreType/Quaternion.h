#pragma once
#include <DirectXMath.h>

namespace cha
{
	/// @brief 四元数
	struct Quaternion : DirectX::XMFLOAT4
	{
		/// @brief ジンバルロック回避のためのしきいち
		static inline const float GIMBAL_LOCK_THRESHOLD{ 0.0001f };

		/// @brief XMFLOAT4のコンストラクタを引き継ぐ
		using DirectX::XMFLOAT4::XMFLOAT4;

		/// @brief XMVECTORからコピーしてくる
		/// @param _v XMVECTOR
		inline Quaternion(const DirectX::XMVECTOR& _v)
		{
			DirectX::XMStoreFloat4(this, _v);
		}

		/// @brief XMVECTORへ暗黙的にキャストする
		inline operator DirectX::XMVECTOR() const noexcept
		{
			return DirectX::XMLoadFloat4(this);
		}

		/// @brief クォータニオンをオイラー角へ変換する
		/// @return オイラー角のXMVECTOR
		inline DirectX::XMVECTOR ToEuler() const
		{
			using namespace DirectX;

			XMMATRIX m{ DirectX::XMMatrixRotationQuaternion(*this) };
			XMFLOAT4X4 matF44{};
			XMStoreFloat4x4(&matF44, m);

			float pitch{ asinf(-matF44._32) };
			float yaw{};
			float roll{};

			// ジンバルロックしないなら
			if (cosf(pitch) > GIMBAL_LOCK_THRESHOLD)
			{
				yaw = atan2f(matF44._31, matF44._33);
				roll = atan2f(matF44._12, matF44._22);
			}
			else
			{
				yaw = 0.0f;
				roll = atan2f(-matF44._21, matF44._11);
			}

			return { pitch, yaw, roll, 0.0f };
		}

		/// @brief オイラー角からクォータニオンを生成する
		/// @param _euler オイラー角
		/// @return クォータニオン
		static inline Quaternion FromEuler(const DirectX::XMVECTOR& _euler)
		{
			DirectX::XMFLOAT3 eulerF3{};
			DirectX::XMStoreFloat3(&eulerF3, _euler);
			return DirectX::XMQuaternionRotationRollPitchYaw(
				eulerF3.x, eulerF3.y, eulerF3.z);
		}

		/// @brief 単位クォータニオンを取得する
		/// @return 単位クォータニオン
		static inline Quaternion Identity()
		{
			return DirectX::XMQuaternionIdentity();
		}
	};
}

