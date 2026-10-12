#pragma once
#include "../../BasePch.h"
#include "IResourceRegistry.h"
#include "../ResourceTraits.h"

namespace cha
{
	/// @brief リソースレジストリのベース
	/// @tparam ResourceDataT リソースデータの構造体型
	/// @tparam ResourceLoadConfigT リソース読み込む設定型 (デフォルトでファイルパス)
	template<typename ResourceDataT, typename ResourceLoadConfigT = fs::path>
	class ResourceRegistryBase : public IResourceRegistry
	{
	public:
		using DataT = ResourceDataT;
		using LoadConfigT = ResourceLoadConfigT;

	private:
		struct Slot
		{
			ResourceDataT data{};
			uint32_t generation{ 1 };  /// 世代番号 1から開始
			bool isAlive{ false };     ///< 使用中フラグ
		};

		// ビットマスク用の定数
		static constexpr uint32_t INDEX_BITS = 20;
		static constexpr uint32_t GEN_BITS = 12;
		static constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1u; // 0x000FFFFF
		static constexpr uint32_t GEN_MASK = (1u << GEN_BITS) - 1u;     // 0x00000FFF

	public:
		ResourceRegistryBase();
		virtual ~ResourceRegistryBase() = default;

	// リソースプールをクリアする（安全に破棄するため）
	inline void Clear()
	{
		registry_.clear();
		pathIndex_.clear();
		succededLast_ = true;
		// 最低1つはスロットを確保しておく従来の挙動を維持
		registry_.emplace_back();
	}

		/// @brief リソースを読み込む or 取得する
		/// @param _config 設定
		/// @return ハンドル (失敗時無効ハンドル)
		inline virtual Handle AddOrGet(ResourceLoadConfigT&& _config) final
		{
		// 重複読み込み防止: LoadConfig が fs::path の場合は既存の登録を検索してハンドルを返す
		if constexpr (std::is_same_v<LoadConfigT, fs::path>)
		{
			const std::string key = fs::path(_config).lexically_normal().string();
			auto it = pathIndex_.find(key);
			if (it != pathIndex_.end())
			{
				return it->second;
			}
		}

			if (succededLast_)
			{
				// 前回成功していたなら新規作成
				registry_.push_back({});
			}
			else
			{
				// 失敗していたなら使い回す
				registry_.back() = std::move(ResourceDataT{});
			}
			ResourceDataT& data = registry_.back();

			if (succededLast_ = TryLoad(std::move(_config), data))
			{
			if constexpr (std::is_same_v<LoadConfigT, fs::path>)
			{
				const std::string key = fs::path(_config).lexically_normal().string();
				pathIndex_[key] = static_cast<Handle>(registry_.size() - 1);
			}
				return static_cast<Handle>(registry_.size() - 1);
			}
			else
			{
				return INVALID_HANDLE;
			}
		}

		/// @brief リソースを読み込む or 取得する
		/// @param _file ファイルパス
		/// @return ハンドル (失敗時無効ハンドル)
#if 0
		inline virtual Handle AddOrGet(const fs::path& _file) final
		{
			if (succededLast_)
			{
				// 前回成功していたなら新規作成
				registry_.push_back({});
			}
			else
			{
				// 失敗していたなら使い回す
				registry_.back() = std::move(ResourceDataT{});
			}
			ResourceDataT& data = registry_.back();

			if (succededLast_ = TryLoad(_file, data))
			{
				return static_cast<Handle>(registry_.size() - 1);
			}
			else
			{
				return INVALID_HANDLE;
			}
		}
#endif

		/// @brief リソースを追加する
		/// @param _data リソースデータ構造体
		/// @return ハンドル
		inline Handle Add(ResourceDataT&& _data)
		{
			if (succededLast_)
			{
				// 前回成功していたなら新規作成
				registry_.emplace_back(std::move(_data));
			}
			else
			{
				// 失敗していたなら使い回す
				registry_.back() = std::move(_data);
			}
			succededLast_ = true;
			return static_cast<Handle>(registry_.size() - 1);
		}

		/// @brief リソースを取得する
		/// @param _handle ハンドル
		/// @return リソースの参照ポインタ
		inline ResourceDataT& GetResource(const Handle _handle)
		{
			return registry_[_handle];
		}

	protected:
		/// @brief 試しにファイルからリソースをロードする
		/// @param _file リソースファイルのパス
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
#if 0
		inline virtual bool TryLoad(const fs::path& _file, ResourceDataT& _out) { return false; }
#endif

		/// @brief 試しに読み込み設定からリソースをロードする
		/// @param _config 読み込み設定
		/// @param _out ロード出力先
		/// @return ロードに成功 true / false
		inline virtual bool TryLoad(ResourceLoadConfigT&& _config, ResourceDataT& _out) { return false; }

	private:
		// 前回読み込みに成功したか
		bool succededLast_{ true };
		// リソースのプール
		std::vector<ResourceDataT> registry_{};

		// ファイルパスベースのリソースに対する重複検出用インデックス
		// LoadConfigT が fs::path のときのみ利用する
		std::unordered_map<std::string, Handle> pathIndex_{};
	};
	template<typename ResourceDataT, typename ResourceLoadConfigT>
	inline ResourceRegistryBase<ResourceDataT, ResourceLoadConfigT>::ResourceRegistryBase()
	{
		registry_.emplace_back();
	}
}
