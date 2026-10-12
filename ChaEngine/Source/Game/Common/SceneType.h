#pragma once

/// @brief シーンの種類
using SceneType = uint8_t;

/// @brief シーンのenum
enum SceneType_ : SceneType
{
	SceneType_Title,            // タイトルシーン
	SceneType_Lobby,            // ロビーシーン
	SceneType_Play,             // プレイシーン
	SceneType_Result,           // 結果シーン
	SceneType_AvoidanceEditor,  // 弾幕耐久編集シーン
	SceneType_Test,             // テストシーン
};
