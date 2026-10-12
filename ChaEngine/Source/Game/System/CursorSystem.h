#pragma once
#include <Base/Base.h>

/// @brief カーソルモード
enum CursorMode
{
	CursorMode_Free,        // 制限なし
	CursorMode_WindowLock,  // ウィンドウ内に固定
	CursorMode_CenterLock,  // 中央固定
};

/// @brief カーソルシステムのデータ
struct CursorSystemData
{
	float mouseSensitivity{ 0.2f };  // マウス感度

	float deltaX{};  // 生の移動量
	float deltaY{};  // 生の移動量
	float moveX{};   // 感度込みの移動量
	float moveY{};   // 感度込みの移動量

	CursorMode mode{ CursorMode_Free };  // カーソルのモード
	Vec2Int currentPosition{};           // 現フレームの位置
	Vec2Int previousPosition{};          // 前フレームの位置

	HWND hWnd{ nullptr };  // ウィンドウハンドル
};

/// @brief カーソルシステム
struct CursorSystem : SystemObjectBase<CursorSystem, CursorSystemData>
{
	/// @brief インスタンスされて最初の1フレームだけ呼ばれる処理
	void Start() override;

	/// @brief 毎フレームの更新処理
	void Update() override;

	/// @brief カーソルモードを変更する
	/// @param _mode 変更先のカーソルモード
	inline void ChangeMode(const CursorMode _mode) { mode = _mode; }

	/// @brief カーソルの表示をセットする
	/// @param _visible 表示する true / false
	void SetVisible(bool _visible);

	/// @brief カーソルが表示されているか取得
	/// @return 表示されている true / false
	bool IsVisible() const;

	/// @brief マウスカーソルのクライアント内座標を取得する
	/// @return クライアント内座標
	Vec2Int GetPosition() const;

private:
	/// @brief カーソル座標を更新
	void UpdatePosition();

	/// @brief 更新処理、カーソルモード: フリー
	void UpdateFree(const RECT& _rect);

	/// @brief 更新処理、カーソルモード: ウィンドウ内固定
	void UpdateWindowLock(const RECT& _rect);

	/// @brief 更新処理、カーソルモード: 中央固定
	void UpdateCenterLock(const RECT& _rect);
};
