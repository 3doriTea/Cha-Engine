#pragma once
#include <Game/Game.h>

/// @brief 杭オブジェクトのデータ
struct StakeData
{
	inline StakeData(const std::string_view _name) :
		name{ _name }
	{}

	Component<Name> name{};
	Component<Transform> transform{};
	Component<ImTransform> imTransform{};
};

/// @brief 杭オブジェクト
struct Stake : ObjectBase<StakeData>
{
	using ObjectBase<StakeData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
};
