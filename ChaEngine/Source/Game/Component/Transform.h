#pragma once
#include <Base/Base.h>
#include <Base/Manager/Dx11Manager.h>

struct TransformMaterialProperty_
{
	Mat4x4 world{ Mat4x4::Identity() };
	Mat4x4 normal{ Mat4x4::Identity() };
};

using TransformMaterialProperty = MaterialProperty<TransformMaterialProperty_>;

/// @brief 座標系コンポーネント
struct Transform : ComponentBase<Transform>
{
	Transform() = default;
	inline Transform(const Vec3 _position) :
		position{ _position }
	{}
	inline Transform(const GameObject _parent) :
		parent{ _parent }  // 親オブジェクトを指定する
	{}

	virtual ~Transform() = default;

	/// @brief ローカル座標系の右向きベクトル
	/// @return 単位ベクトル
	inline Vec3 Right()   const { return DirectX::XMVector3Normalize(world.r[0]); }
	/// @brief ローカル座標系の上向きベクトル
	/// @return 単位ベクトル
	inline Vec3 Up()      const { return DirectX::XMVector3Normalize(world.r[1]); }
	/// @brief ローカル座標系の前向きベクトル
	/// @return 単位ベクトル
	inline Vec3 Forward() const { return DirectX::XMVector3Normalize(world.r[2]); }
	/// @brief ローカル座標系の左向きベクトル
	/// @return 単位ベクトル
	inline Vec3 Left()    const { return -Right(); }
	/// @brief ローカル座標系の下向きベクトル
	/// @return 単位ベクトル
	inline Vec3 Down()    const { return -Up(); }
	/// @brief ローカル座標系の後ろ向きベクトル
	/// @return 単位ベクトル
	inline Vec3 Back()    const { return -Forward(); }

	/// @brief ワールド座標を取得
	/// @return ワールド座標
	inline Vec3 WorldPosition() const { return world.r[3]; }

	/// @brief Transform のマテリアルを取得
	/// @return マテリアルの参照ポインタ
	virtual inline const Resource<ResourceType::Material>& GetMaterial()
	{
		MaterialResource& transformMaterial = material.Get();
		transformMaterial.slot = 2;
		if (!transformMaterial.pProperty)
		{
			transformMaterial.SetProperty<TransformMaterialProperty>();
		}

		TransformMaterialProperty* pProperty =
			static_cast<TransformMaterialProperty*>(transformMaterial.pProperty.get());

		(*pProperty)->world = world;
		(*pProperty)->normal = normal;

		return material;
	}

	/// @brief 血筋をたどって一族を更新する
	void UpdateFamily();
	/// @brief 自分自身のみを更新する
	void UpdateSelf();

	Vec3 position      { Vec3::Zero() };            ///< 座標
	Quaternion rotation{ Quaternion::Identity() };  ///< 回転
	Vec3 scale         { Vec3::One() };             ///< 拡縮

	Mat4x4 world{ Mat4x4::Identity() };       ///< ワールド行列
	Mat4x4 normal{ Mat4x4::Identity() };      ///< 回転のみワールド行列
	Mat4x4 local{ Mat4x4::Identity() };       ///< ローカル行列
	Mat4x4 localNormal{ Mat4x4::Identity() }; ///< 回転のみローカル行列

	GameObject parent{};  ///< 親のゲームオブジェクト

protected:
	Resource<ResourceType::Material> material{ MaterialResource::InstanceEmpty<TransformMaterialProperty>() };
};
