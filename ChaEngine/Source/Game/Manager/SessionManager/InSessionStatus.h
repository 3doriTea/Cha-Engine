#pragma once
#include <Game/Game.h>

/// @brief セッション内のステータス
struct InSessionStatus
{
	/// @brief プレイヤーのステータス
	struct PlayerStatus
	{
		Vec3 position{ 0, 10, 0 };  ///< 座標
		bool updatedFlag{ false };  ///< 更新時のフラグ false / true 切り替え用
	};

	std::string joinRoomName{};              ///< 参加する部屋名
	std::string name{};                      ///< プレイヤー名
	std::vector<std::string> memberNames{};  ///< ロビー内メンバー名一覧

	bool updatedFlag{ false };  ///< 更新確認用フラグ false / true 切り替え用
	std::unordered_map<std::string, PlayerStatus> playersStatus{};  ///< プレイヤーの座標情報
};
