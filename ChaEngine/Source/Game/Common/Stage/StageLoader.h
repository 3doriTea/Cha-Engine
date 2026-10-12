#pragma once
#include <Game/Game.h>

/// @brief ステージを読み込むクラス
class StageLoader
{
public:
	/// @brief 初期化処理
	StageLoader() = default;
	/// @brief 解放処理
	~StageLoader() = default;

	/// @brief ブロックを設置するときの処理
	/// @tparam Func void(Vec3Int, BlockType)
	/// @param _func void(Vec3Int, BlockType)
	template<typename Func>
	void OnSetBlock(Func&& _func);

	/// @brief 試しに読み込む
	/// @param _filePath ファイルのパス
	/// @return 読み込みに成功 true / false
	bool TryLoad(const fs::path& _filePath);
	/// @brief 試しに保存する
	/// @param _filePath ファイルパス
	/// @return 保存に成功 true / false
	bool TrySave(const fs::path& _filePath);
};

#include "StageLoader.inl"
