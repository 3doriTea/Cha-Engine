#pragma once
#include <Game/Game.h>

/// @brief 字幕表示UI
struct MessageAreaData
{
	Component<UITransform> transform{};   // UI座標系
	Component<UIImage> image{};           // UI画像
	Component<UIRaycastTarget> target{};  // 当たり判定用

	bool enabled{ false };  // 有効か

	float timeLeft{};  // 残りの表示時間

	// 総再生時間
	float totalTyepingTime{};

	std::string showText{};  // 表示する文字列
	std::vector<size_t> characterIndeces{};

	bool isTyping{};  // アニメーション中

	float charTimeSec{ 0.1f };  // 1文字にかける時間
	Vec2 scale{ 1186, 277 };    // UIのスケール
	Vec2 begin{ 207, 560 };     // UIの開始位置

	// メッセージテキストオブジェクト
	GameObject objMessageText{};
	GameObject objMessageAuthor{};
};

/// @brief 字幕表示UI
struct MessageArea : ObjectBase<MessageAreaData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 字幕ボックスを更新する
	void Update() override;

	/// @brief 文字列を表示する
	void ShowText(const std::string& _content, const std::string& _author);

private:
	/// @brief 隠す
	void Hide();

	/// @brief 表示する
	void Show();
};
