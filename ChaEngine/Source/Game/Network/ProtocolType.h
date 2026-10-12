#pragma once
#include <Game/Game.h>

enum struct ProtocolType : uint8_t
{
	JoinRoom = 0,    ///< 部屋を作成・参加 (部屋名)
	JoinOk,          ///< 部屋の参加ok (プレイヤー名)
	LobbyMembers,    ///< ロビー内メンバー一覧
	ReadySelf,       ///< ロビー内で準備ok
	StartGame,       ///< ゲーム開始
	StartedGame,     ///< ゲームが開始された
	UpdatePlayer,    ///< プレイヤーの情報更新
	UpdatedPlayers,  ///< 全プレイヤーの更新情報
	LoadedTerrain,   ///< 地形のチャンク情報を取得
	FinishGame,      ///< ゲームが終わった
	ResultInfo,      ///< 結果情報
	LeaveRoom,       ///< 部屋から離脱

	Idk,
};
