#pragma once
#include "ResourceRegistryBase.h"
#include "Model/ModelParts.h"
#include "Model/ModelVertex.h"
#include "../Resource.h"
#include "Material.h"
#include <ufbx/ufbx.h>


namespace cha
{
	// アニメーションのクリップ(Run, Walk など)
	struct AnimationClip
	{
		std::string name{};
		float duration{};
	};

	/// @brief ボーンの情報
	struct BoneInfo
	{
		std::string name;
		int parentIndex;        // 親ボーンの配列インデックス
		Mat4x4 invBindMatrix;   // 逆バインドポーズ行列
		Mat4x4 localTransform;  // 初期ポーズのTRS
	};

	/// @brief モデルリソース
	struct ModelResource
	{
		ModelResource() = default;

		// コピーは禁止
		ModelResource(const ModelResource&) = delete;
		ModelResource& operator=(const ModelResource&) = delete;

		// ムーブを許可
		ModelResource(ModelResource&& _other) noexcept :
			file{ std::move(_other.file) },
			subParts{ std::move(_other.subParts) },
			isSkinnedModel{ _other.isSkinnedModel },
			bones{ std::move(_other.bones) },
			animations{ std::move(_other.animations) },
			pRawScene{ _other.pRawScene },  // ufbxのシーンポインタは引き継ぐ
			pBoneConstantBuffer{ std::move(_other.pBoneConstantBuffer) }
		{
			_other.pRawScene = nullptr;  // 移動させた元が二重解放しないようnullptrに
		}

		// ムーブを許可
		ModelResource& operator=(ModelResource&& _other) noexcept
		{
			if (this != &_other)
			{
				// 自身の持つ古いシーンがあれば先に解放しておく
				if (pRawScene) ufbx_free_scene(pRawScene);

				file = std::move(_other.file);
				subParts = std::move(_other.subParts);
				isSkinnedModel = _other.isSkinnedModel;
				bones = std::move(_other.bones);
				animations = std::move(_other.animations);
				pRawScene = _other.pRawScene;
				pBoneConstantBuffer = std::move(_other.pBoneConstantBuffer);

				_other.pRawScene = nullptr;
			}
			return *this;
		}

		~ModelResource()
		{
			if (pRawScene)  // ちゃんと解放
			{
				ufbx_free_scene(pRawScene);
				pRawScene = nullptr;
			}
		}
		
		inline HashValue GetHash() const
		{
			HashValue hash{};
			HashCombine(hash, file.string());
			return hash;
		}

		/// @brief 仮のアニメーション更新描画処理
		/// @param _deltaTime デルタタイム
		/// @param _animIndex アニメーションインデックス
		void Draw(const float _deltaTime, const size_t _animIndex);

		/// @brief キーフレームではないアニメーションの適用
		/// @param _deltaTime アニメーション時間
		/// @param _animIndex アニメーションインデックス
		void UpdateAnim(const float _deltaTime, const int _animIndex);

		fs::path file{};  // モデルファイル

		std::vector<ModelParts> subParts{};

		// アニメーション対応用に拡張
		bool isSkinnedModel = false;
		std::vector<BoneInfo> bones{};
		std::vector<AnimationClip> animations{};

		size_t playingAnimIndex{ 0 };   // 再生中のアニメーションインデックス
		float currentAnimTime{ 0.0f };  // アニメーションの再生時間

		ufbx_scene* pRawScene{ nullptr };  // シーンの情報をそのままもっちゃう

		// 各モデル個別のボーン行列をGPUへ送るための定数バッファを追加！
		ComPtr<ID3D11Buffer> pBoneConstantBuffer{ nullptr };
	};

	/// @brief モデルリソースを格納
	class ModelRegistry : public ResourceRegistryBase<ModelResource>
	{
	public:
		ModelRegistry() = default;
		~ModelRegistry() = default;

		bool TryLoad(fs::path&& _file, ModelResource& _out) override;

	private:
		/// @brief FBXモデルを試しにロードする
		/// @param _file fbxフィアルパス
		/// @param _out 出力先
		/// @return ロードに成功 true / false
		bool TryLoadFbx(const fs::path& _file, ModelResource& _out);

		bool TryLoadParts(
			ufbx_mesh* _pMesh,
			ufbx_mesh_part* _pPart,
			ModelParts& _out) const;

		bool TryCreateVertexBuffer(
			ComPtr<ID3D11Buffer>& _p,
			const std::vector<ModelVertex> _vertices) const;
		bool TryCreateIndexBuffer(
			ComPtr<ID3D11Buffer>& _p,
			const std::vector<uint32_t> _indices) const;

		// ボーン専用の定数バッファを作成するヘルパーを追加
		bool TryCreateBoneBuffer(
			ComPtr<ID3D11Buffer>& _p) const;
	};

	template<> struct ResourceTraits<ResourceType::Model>
	{
		using DataT = ModelResource;
		using RegistryT = ModelRegistry;
		using LoadConfigT = fs::path;
	};
}
