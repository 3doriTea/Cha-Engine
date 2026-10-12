#pragma once
#include <Base/Base.h>
#include "../Core/Resource.h"

namespace cha
{
	class Dx11Manager;

	/// @brief ライトの管理
	class LightManager : public IManager
	{
	public:
		LightManager(Dx11Manager& _dx11Manager);
		~LightManager() = default;

		/// @brief ライトの更新処理
		/// @return 
		bool Update();

		/// @brief ライトの情報をコンスタントバッファに送る
		/// @return 
		bool Bind();

		/// @brief 描画する座標を設定する
		inline void SetPosition(const Vec3 _position) { centerPosition = _position; }

	private:
		Vec3 centerPosition{};  // ライトの中心座標
		Vec3 direction_{ Vec3::Down() };  // ライトの方向
		float shadowRange{ 80.0f };  // 影を描画する箱の大きさ
		Resource<ResourceType::Material> lightMaterial_{};  // ライト用マテリアル

		float nearClipDistance{ 1.0f };
		float farClipDistance{ 200.0f };

		Dx11Manager& dx11Manager_;
	};
}
