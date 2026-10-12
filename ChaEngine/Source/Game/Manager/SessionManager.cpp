#include "SessionManager.h"
#include <ranges>

void SessionManager::Reset()
{
	inSessionStatus_ = {};
}

void SessionManager::SetPlayerName(const std::string_view _name)
{
	inSessionStatus_.name = _name;
}

void SessionManager::SetMemberNames(const std::string_view _names)
{
	inSessionStatus_.memberNames.clear();

	// 文字列を改行で分解してく
	for (auto&& line_range : _names | std::views::split('\n'))
	{
		std::string_view line{ line_range.begin(), line_range.end() };

		// CRLF 対策
		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		inSessionStatus_.memberNames.push_back(std::string{ line });
	}
}

void SessionManager::SetJoinRoomName(const std::string_view _name)
{
	inSessionStatus_.joinRoomName = _name;
}

void SessionManager::UpdatePlayers(const std::string_view _names, SizedBuffer<float>& _positions)
{
	inSessionStatus_.updatedFlag = !inSessionStatus_.updatedFlag;
	auto& playerStatus = inSessionStatus_.playersStatus;

	size_t i = 0;
	// 文字列を改行で分解してく
	for (auto&& line_range : _names | std::views::split('\n'))
	{
		if (Vec3::AT_MAX * i >= _positions.Size())
		{
			VCU_LOG("取得した名前分の座標データのサイズが足りない({}個以上必要)", Vec3::AT_MAX * i);
			break;
		}

		std::string_view line{ line_range.begin(), line_range.end() };

		// CRLF 対策
		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		InSessionStatus::PlayerStatus& playerStatus = inSessionStatus_.playersStatus[std::string{ line }];
		playerStatus.position = Vec3
		{
			_positions[i * Vec3::AT_MAX + Vec3::AT_X],
			_positions[i * Vec3::AT_MAX + Vec3::AT_Y],
			_positions[i * Vec3::AT_MAX + Vec3::AT_Z],
		};
		playerStatus.updatedFlag = !playerStatus.updatedFlag;  // 更新フラグを反転

		VCU_LOG("Update[{}] = ({}, {}, {})", line, playerStatus.position.x, playerStatus.position.y, playerStatus.position.z);

		i++;
	}

	// 更新フラグが不一致は削除していく
	std::erase_if(inSessionStatus_.playersStatus, [this](const auto& nameAndStatus)
	{
		if (nameAndStatus.second.updatedFlag != inSessionStatus_.updatedFlag)
			VCU_LOG("削除オオオ{}", nameAndStatus.first);
		return nameAndStatus.second.updatedFlag != inSessionStatus_.updatedFlag;
	});
}

const InSessionStatus::PlayerStatus& SessionManager::RefPlayerStatus(const std::string& _playerName) const
{
	return inSessionStatus_.playersStatus.at(_playerName);
}
