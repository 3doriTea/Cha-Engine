
#include "../Mathf.h"
#include "Model.h"
#include "../../ManagerRegistry.h"
#include "../../Manager/Dx11Manager.h"
#include "Surface.h"
#include "Model/ModelMaterial.h"

namespace
{
	/// <summary>
	/// ufbx_matrixをMat4x4に変換
	/// </summary>
	/// <param name="_from">ufbx_matrix</param>
	/// <returns>Mat4x4</returns>
	static inline cha::Mat4x4 ConvertToMat4x4(const ufbx_matrix& _from)
	{
		return cha::Mat4x4
		{
			static_cast<float>(_from.m00), static_cast<float>(_from.m10), static_cast<float>(_from.m20), 0.0f,  // 1行目: X軸
			static_cast<float>(_from.m01), static_cast<float>(_from.m11), static_cast<float>(_from.m21), 0.0f,  // 2行目: Y軸
			static_cast<float>(_from.m02), static_cast<float>(_from.m12), static_cast<float>(_from.m22), 0.0f,  // 3行目: Z軸
			static_cast<float>(_from.m03), static_cast<float>(_from.m13), static_cast<float>(_from.m23), 1.0f,  // 4行目: 移動(Translation)
		};
	}

	/// @brief ufbx_vec4をfloat4に変換
	/// @param _from ufbx_vec4
	/// @return cha::Float4
	static inline cha::Float4 ConvertToFloat4(const ufbx_vec4& _from)
	{
		return cha::Float4
		{
			static_cast<float>(_from.x), static_cast<float>(_from.y), static_cast<float>(_from.z), static_cast<float>(_from.w)
		};
	}
}

bool cha::ModelRegistry::TryLoad(fs::path&& _file, ModelResource& _out)
{
	if (_file.extension() == ".fbx")
	{
		return TryLoadFbx(_file, _out);
	}
	
	VCU_ASSERT(false && "未対応のモデルファイル拡張子");
	return false;
}

bool cha::ModelRegistry::TryLoadFbx(const fs::path& _file, ModelResource& _out)
{
	ufbx_error error{};
	ufbx_load_opts options{};

	options.target_axes = ufbx_axes_left_handed_y_up;  // 左手座標系

	options.target_unit_meters = 1.0f;

	ufbx_scene* pScene{ ufbx_load_file(_file.string().c_str(), &options, &error) };
	VCU_ASSERT(pScene && "FBXシーンの読み込みに失敗");
	if (!pScene)
	{
		return false;
	}

	_out.file = _file;

	for (ufbx_node* pNode : pScene->nodes)
	{
		VCU_LOG("{}", pNode->name.data);

		if (ufbx_mesh* pMesh{ pNode->mesh }; pMesh)
		{
			for (auto& part : pMesh->material_parts)
			{
				ModelParts parts{};
				parts.name = pNode->name.data;

				if (bool succeed = TryLoadParts(pNode->mesh, &part, parts); !succeed)
				{
					VCU_ASSERT(false && "パーツ読み込みに失敗");
					return false;
				}

				{  // マテリアル作成
					MaterialResource material{};

					std::unique_ptr<ModelMaterialProperty> pModelProperty = std::make_unique<ModelMaterialProperty>();
					pModelProperty->Get().toWorldMatrix = ConvertToMat4x4(pNode->node_to_parent);

					// TODO: もしかしたら part.index が不適正かも

					if (pNode->materials.count == 0)
					{
						continue;  // マテリアル 0 なら無視
					}

					ufbx_material* pUfbxMaterial{ pNode->materials[part.index] };

					pModelProperty->Get().hasTextureSkinAnim.x = FALSE;
					pModelProperty->Get().hasTextureSkinAnim.y = pScene->skin_deformers.count > 0;
					pModelProperty->Get().hasTextureSkinAnim.z = pScene->anim_stacks.count > 0;

					// マテリアルのプロパティを出力する
					/*for (auto& prop : pUfbxMaterial->props.props)
					{
						VCU_LOG("prop name :{}", prop.name.data);
					}*/

					{  // ディフューズカラー
						ufbx_prop* pProp = ufbx_find_prop(&pUfbxMaterial->props, "DiffuseColor");
						if (pProp)
						{
							pModelProperty->Get().baseColor = ConvertToFloat4(pProp->value_vec4);
						}
					}
				

					// テクスチャがあるなら探す
					if (pUfbxMaterial->textures.count > 0)
					{
						ufbx_material_texture textureInMaterial{ pUfbxMaterial->textures[0] };
						ufbx_texture* pTexture = textureInMaterial.texture;

						if (pTexture && pTexture->filename.length > 0)
						{
							pModelProperty->Get().hasTextureSkinAnim.x = TRUE;

							std::string_view textureFileName{ pTexture->filename.data };

							// 💡 埋め込みデータ（Embedded）が存在するかチェック
							if (pTexture->content.size > 0 && pTexture->content.data != nullptr)
							{
								// ✨ 【埋め込み対応】メモリの生バイナリからテクスチャを作成する
								// ※必要に応じて、独自マテリアルクラスに「メモリからロードする機能」を追加してください
								material.texture =
								{
									TextureLoadConfig
									{
										pTexture->content.data,
										pTexture->content.size,
										textureFileName // 拡張子を判別するために元のファイル名も渡す
									}
								};
							}
							else
							{
								// 【既存の処理】外部ファイル名からロードする
								material.texture = { textureFileName };
							}
						}
					}
					else
					{
						
					}


					material.TryLoad(std::move(pModelProperty));

					parts.material = Resource<ResourceType::Material>{ std::move(material) };
				}

				MaterialResource& transformMaterial = parts.transformMaterial.Get();
				transformMaterial.slot = 3;
				ModelTransformMaterialProperty* pProperty =
					static_cast<ModelTransformMaterialProperty*>(transformMaterial.pProperty.get());

				(*pProperty)->toWorldMatrix =
					//cha::Mathf::RemoveScale(
					//ConvertToMat4x4(pNode->geometry_to_world)
					ConvertToMat4x4(pNode->geometry_to_world)
				//)
				;

				_out.subParts.emplace_back(std::move(parts));
			}
		}
	}

	// NOTE: ufbxのシーンをそのままモデルで使うため、解放はモデル構造体の解放タイミングで行う
	//     : ufbx_free_scene(pScene);
	_out.pRawScene = pScene;

	if (pScene->skin_deformers.count > 0)
	{
		_out.isSkinnedModel = true;
		if (!TryCreateBoneBuffer(_out.pBoneConstantBuffer))
		{
			return false; // 作成失敗
		}
	}

	return true;
}

bool cha::ModelRegistry::TryLoadParts(
	ufbx_mesh* _pMesh,
	ufbx_mesh_part* _pPart,
	ModelParts& _out) const
{
	std::vector<ModelVertex> vertices{};
	std::vector<uint32_t> polyIndices;
	polyIndices.resize(_pMesh->max_face_triangles * 3);

	// 各マテリアルごとにポリゴンを処理
	for (uint32_t polyIndex : _pPart->face_indices)
	{
		ufbx_face poly{ _pMesh->faces[polyIndex] };

		// 面を三角化する
		uint32_t polyCount
		{
			ufbx_triangulate_face(
				polyIndices.data(), polyIndices.size(), _pMesh, poly)
		};

		for (uint32_t i = 0; i < polyCount * 3; i++)
		{
			uint32_t index{ polyIndices[i] };

			ModelVertex v{};

			// 頂点の座標を取得
			v.position = Vec3{ _pMesh->vertex_position[index].v };
			// 頂点の法線を取得
			v.normal = Vec3{ ufbx_get_vertex_vec3(&_pMesh->vertex_normal, index).v };
			// 頂点のUVを取得
			v.uv = Vec2{ ufbx_get_vertex_vec2(&_pMesh->vertex_uv, index).v };
			v.uv.y = 1.0f - v.uv.y;

			// 頂点にスキンメッシュアニメーション用のウェイトインデックスとウェイトを割り当てる
			for (int b = 0; b < 4; b++)
			{
				v.boneIndices[b] = 0;
				v.boneWeights[b] = 0.0f;
			}

			// この頂点が属している元のメッシュ頂点インデックスを取得
			uint32_t originalVertexIndex = _pMesh->vertex_indices[index];

			// ufbxの正しいスキンデフォーマーへのアクセス
			if (_pMesh->skin_deformers.count > 0)
			{
				// TODO: 理論頂点ってなんぞや
				ufbx_skin_deformer* pSkin = _pMesh->skin_deformers.data[0];  // でフォーマーは(理論頂点?)を0として取得

				if (pSkin)
				{
					// 各頂点に対してどのweightがついているかの範囲
					ufbx_skin_vertex skinVertex = pSkin->vertices[originalVertexIndex];

					uint32_t numWeights = skinVertex.num_weights;    // 頂点が影響するウェイトの数
					uint32_t useWeights = std::min(numWeights, 4u);  // 最大4つに制限

					float weightSum = 0.0f;  // ウェイトの影響度の合計
					for (uint32_t b = 0; b < useWeights; b++)
					{
						uint32_t weightIdx = skinVertex.weight_begin + b;  // ウェイトのインデクス
						ufbx_skin_weight skinWeight = pSkin->weights.data[weightIdx];  // ウェイトを参照

						v.boneIndices[b] = skinWeight.cluster_index;
						v.boneWeights[b] = static_cast<float>(skinWeight.weight);  // 影響度
						weightSum += v.boneWeights[b];  // 影響度を加算
					}

					// ウェイトの影響度合計を1に正規化する
					if (weightSum > 0.0f)
					{
						for (uint32_t b = 0; b < 4; b++)
						{
							v.boneWeights[b] /= weightSum;  // 正規化！
						}
					}
					else  // もしウェイトの影響度合計が0なら割れないため1.0fする
					{
						v.boneWeights[0] /= 1.0f;
					}
				}
				else  // もしスキン情報がないなら 1.0f をべた張り
				{
					v.boneWeights[0] = 1.0f;
				}
			}

			vertices.push_back(v);
		}
	}

	VCU_ASSERT(vertices.size() == _pPart->num_triangles * 3
		&& "メッシュの三角化に失敗");

	// 頂点ストリーム
	ufbx_vertex_stream streams[]
	{
		{ vertices.data(), vertices.size(), sizeof(ModelVertex) },
	};

	std::vector<uint32_t> indices{};
	indices.resize(_pPart->num_triangles * 3);

	// 頂点重複を削除し、インデックスを作成
	size_t verticesCount
	{
		ufbx_generate_indices(streams, 1, indices.data(), indices.size(), nullptr, nullptr)
	};

	// 前詰めされたからリサイズする
	vertices.resize(verticesCount);

	SurfaceResource mesh{};

	if (TryCreateVertexBuffer(mesh.pVertexBuffer, vertices)
		&& TryCreateIndexBuffer(mesh.pIndexBuffer, indices))
	{
		mesh.size = sizeof(ModelVertex);
		mesh.vertexCount = static_cast<UINT>(vertices.size());
		mesh.indexCount = static_cast<UINT>(indices.size());
		_out.mesh = { std::move(mesh) };
		return true;  // 成功
	}

	return false;  // どちらかバッファ作成失敗
}

bool cha::ModelRegistry::TryCreateVertexBuffer(ComPtr<ID3D11Buffer>& _p, const std::vector<ModelVertex> _vertices) const
{
	const D3D11_BUFFER_DESC DESC
	{
		.ByteWidth = static_cast<UINT>(sizeof(ModelVertex) * _vertices.size()),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = NULL,
		.StructureByteStride = 0,
	};

	D3D11_SUBRESOURCE_DATA data
	{
		.pSysMem = _vertices.data(),
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref([&hResult, &_p, DESC, data](const ComPtr<ID3D11Device> _pDevice)
	{
		hResult = _pDevice->CreateBuffer(&DESC, &data, _p.GetAddressOf());
	});
	VCU_ASSERT(SUCCEEDED(hResult) && "FbxLoadで頂点バッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool cha::ModelRegistry::TryCreateIndexBuffer(
	ComPtr<ID3D11Buffer>& _p,
	const std::vector<uint32_t> _indices) const
{
	const D3D11_BUFFER_DESC DESC
	{
		.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * _indices.size()),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = NULL,
		.StructureByteStride = 0,
	};

	D3D11_SUBRESOURCE_DATA data
	{
		.pSysMem = _indices.data(),
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref([&hResult, &_p, DESC, data](const ComPtr<ID3D11Device> _pDevice)
	{
		hResult = _pDevice->CreateBuffer(&DESC, &data, _p.GetAddressOf());
	});
	VCU_ASSERT(SUCCEEDED(hResult) && "FbxLoadでインデックスバッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool cha::ModelRegistry::TryCreateBoneBuffer(ComPtr<ID3D11Buffer>& _p) const
{
	// シェーダー側の MAX_BONES (256) と完全にサイズを合わせる
	const UINT bufferSize = sizeof(Mat4x4) * 256;

	const D3D11_BUFFER_DESC DESC
	{
		.ByteWidth = bufferSize,
		.Usage = D3D11_USAGE_DYNAMIC,             // 毎フレーム書き換える
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,  // 定数バッファとしてバインド
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE, // CPUからの書き込みを許可
		.MiscFlags = NULL,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref([&hResult, &_p, DESC](const ComPtr<ID3D11Device> _pDevice)
	{
		// 初期データは無し（nullptr）で空のバッファを作る
		hResult = _pDevice->CreateBuffer(&DESC, nullptr, _p.GetAddressOf());
	});

	VCU_ASSERT(SUCCEEDED(hResult) && "ボーン用定数バッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}


void cha::ModelResource::Draw(const float _deltaTime, const size_t _animIndex)
{
	ModelResource& _model = *this;

	ufbx_scene* scene = _model.pRawScene;
	if (!scene || scene->anim_stacks.count == 0) return;

	const size_t PREV_PLAYING_ANIM_INDEX{ _model.playingAnimIndex };
	_model.playingAnimIndex = std::min(_animIndex, scene->anim_stacks.count - 1);
	
	ufbx_anim_stack* stack = scene->anim_stacks.data[_model.playingAnimIndex];  // ひとまず0番目のモーション

	// アニメーションが変更されたなら再生時間を戻す
	if (PREV_PLAYING_ANIM_INDEX != _model.playingAnimIndex)
	{
		_model.currentAnimTime = 0.0f;
	}

	// アニメーションの時間を追加して再生
	_model.currentAnimTime += _deltaTime;
	double duration = stack->time_end - stack->time_begin;  // 総再生時間
	// 現在の時間
	double evalTime = fmod(_model.currentAnimTime, duration) + stack->time_begin;

	ufbx_anim* anim = stack->anim;

	// ⭕ 確保サイズは elements.count のままで安全性を維持
	std::vector<DirectX::XMMATRIX> worldMatrices(scene->elements.count, DirectX::XMMatrixIdentity());

	// 階層順にノードのワールド行列を計算
	for (size_t i = 0; i < scene->nodes.count; ++i)
	{
		ufbx_node* node = scene->nodes.data[i];
		ufbx_transform localTRS = ufbx_evaluate_transform(anim, node, evalTime);

		// DirectXMathの型に変換してローカル行列を作成
		DirectX::XMVECTOR t = DirectX::XMVectorSet((float)localTRS.translation.x, (float)localTRS.translation.y, (float)localTRS.translation.z, 1.0f);
		DirectX::XMVECTOR r = DirectX::XMVectorSet((float)localTRS.rotation.x, (float)localTRS.rotation.y, (float)localTRS.rotation.z, (float)localTRS.rotation.w);
		DirectX::XMVECTOR s = DirectX::XMVectorSet((float)localTRS.scale.x, (float)localTRS.scale.y, (float)localTRS.scale.z, 0.0f);
		//{ 1.0f, 1.0f, 1.0f, 1.0f }; 
		DirectX::XMMATRIX localMatrix = DirectX::XMMatrixAffineTransformation(s, DirectX::XMVectorZero(), r, t);

		// ⭕ 修正：自分の element_id の位置に書き込む
		// 親がいる場合は、すでに親のループ時点で書き込みが終わっている `parent->element_id` の行列を確実に乗算する
		if (node->parent)
		{
			worldMatrices[node->element_id] = DirectX::XMMatrixMultiply(localMatrix, worldMatrices[node->parent->element_id]);
		}
		else
		{
			worldMatrices[node->element_id] = localMatrix;
		}
	}

	// -------------------------------------------------------------
	// ② 逆バインド行列を合成して、最終的な「スキン行列」を作る
	// -------------------------------------------------------------
	if (scene->skin_deformers.count == 0) return;
	ufbx_skin_deformer* skin = scene->skin_deformers.data[0];

	std::vector<cha::Mat4x4> skinningMatrices(256);
	size_t numBones = std::min(skin->clusters.count, size_t(256));

	for (size_t i = 0; i < numBones; ++i)
	{
		ufbx_skin_cluster* cluster = skin->clusters.data[i];

		// ufbxから逆バインド行列を取得
		ufbx_matrix m = cluster->geometry_to_bone;

		// ⭕ 修正：ヘルパー関数が弄れないため、ここもこれまで動いていた「ufbxの生の並び」のまま初期化します
		DirectX::XMMATRIX invBind = DirectX::XMMatrixSet(
			(float)m.v[0], (float)m.v[1], (float)m.v[2], 0.0f,
			(float)m.v[3], (float)m.v[4], (float)m.v[5], 0.0f,
			(float)m.v[6], (float)m.v[7], (float)m.v[8], 0.0f,
			(float)m.v[9], (float)m.v[10], (float)m.v[11], 1.0f
		);
		// ⚠️ 前回入れてしまったTranspose（転置）を削除！これまでの挙動に合わせます

		// ⭕ 安全対策：element_id を使って現在のワールド行列を取得
		DirectX::XMMATRIX currentBoneWorld = DirectX::XMMatrixIdentity();
		if (cluster->bone_node && cluster->bone_node->element_id < worldMatrices.size())
		{
			currentBoneWorld = worldMatrices[cluster->bone_node->element_id];
		}

		// ⭕ 修正：掛け算の順序を「今まで動いていた順序（元のコード）」に戻します
		// もしこれで駄目なら、逆に「DirectX::XMMatrixMultiply(currentBoneWorld, invBind);」を試してください
		DirectX::XMMATRIX finalSkinMatrix = DirectX::XMMatrixMultiply(invBind, currentBoneWorld);

		// ⚠️ HLSLの定数バッファへ送るための転置（これはシェーダーの要求なので残す、または以前外していたなら外す）
		finalSkinMatrix = DirectX::XMMatrixTranspose(finalSkinMatrix);

		DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&skinningMatrices[i]), finalSkinMatrix);
	}

	// -------------------------------------------------------------
	// ③ 作成した行列配列を、Modelが持つ専用定数バッファへ転送＆バインド
	// -------------------------------------------------------------
	ManagerRegistry::Ref<Dx11Manager>().Ref([&](const ComPtr<ID3D11DeviceContext> _pContext)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource{};
		HRESULT hr = _pContext->Map(_model.pBoneConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (SUCCEEDED(hr))
		{
			memcpy(mappedResource.pData, skinningMatrices.data(), sizeof(cha::Mat4x4) * 256);
			_pContext->Unmap(_model.pBoneConstantBuffer.Get(), 0);
		}

		_pContext->VSSetConstantBuffers(4, 1, _model.pBoneConstantBuffer.GetAddressOf());
	});
}

void cha::ModelResource::UpdateAnim(const float _deltaTime, const int _animIndex)
{
	playingAnimIndex = _animIndex;

	ModelResource& _model = *this;

	ufbx_scene* pScene = _model.pRawScene;
	// アニメーションスタックがないなら無視
	if (!pScene || pScene->anim_stacks.count == 0)
	{
		return;
	}

	// 現在再生中のアニメーションを取得
	if (playingAnimIndex >= pScene->anim_stacks.count) return;
	ufbx_anim_stack* pAnimStack = pScene->anim_stacks[playingAnimIndex];

	// 再生時間進める
	_model.currentAnimTime += _deltaTime;

	// アニメーションの長さに応じて時間をループ処理
	float duration = static_cast<float>(pAnimStack->time_end - pAnimStack->time_begin);
	float evalTime = static_cast<float>(pAnimStack->time_begin);

	if (duration > 0.0f)
	{
		evalTime += std::fmodf(currentAnimTime, duration);
	}

	// 指定時間に置けるアニメーション状態をサンプリング
	ufbx_error error{};
	ufbx_scene* pEvaluatedScene = ufbx_evaluate_scene(pRawScene, pAnimStack->anim, evalTime, nullptr, &error);
	if (!pEvaluatedScene)
	{
		// エラー処理（必要に応じてログ出力など）
		return;
	}

	if (!isSkinnedModel)
	{
		// 評価されたシーンの各ノードをループ
		for (size_t i = 0; i < pEvaluatedScene->nodes.count; ++i)
		{
			ufbx_node* pEvalNode = pEvaluatedScene->nodes[i];
			if (!pEvalNode->mesh) continue;

			// ufbxのノード名と一致する subParts を探す
			// (Setup時の name の入れ方に応じて、pEvalNode->name.data または pEvalNode->mesh->name.data と比較してください)
			for (auto& part : subParts)
			{
				if (part.name == pEvalNode->name.data)
				{
					// 親子関係が計算済みのワールド行列を取得
					ufbx_matrix rawWorldMat = pEvalNode->node_to_world;

					// 既存の ConvertToMat4x4 ヘルパーを使用して独自の行列型に変換
					cha::Mat4x4 localWorldMatrix = ConvertToMat4x4(rawWorldMat);

					// transformMaterial のプロパティを書き換える
					ModelTransformMaterialProperty* pProp = static_cast<ModelTransformMaterialProperty*>(part.transformMaterial.Get().pProperty.get());
					
					pProp->Get().toWorldMatrix = localWorldMatrix;
					break;
				}
			}
		}
	}
	else
	{
		// 💡 スキンメッシュモデルの場合は、既存のボーン行列配列 (worldMatrices) を計算する処理をここに記述します
		// (以前実装しかけていた skinningMatrices への転送処理などをここに移植すると綺麗に収まります)
	}

	// 評価用の一時的なシーンを必ず解放（メモリリーク防止）
	ufbx_free_scene(pEvaluatedScene);
}














//void cha::ModelResource::Draw(float _deltaTime)
//{
//	ModelResource& _model = *this;
//
//	ufbx_scene* scene = _model.pRawScene;
//	if (!scene || scene->anim_stacks.count == 0) return;
//
//	// -------------------------------------------------------------
//	// ① 再生時間を進めて、ufbxにその瞬間のポーズ（TRS）を自動補間してもらう
//	// -------------------------------------------------------------
//	ufbx_anim_stack* stack = scene->anim_stacks.data[0]; // ひとまず0番目のモーション
//
//	// 💡 本来はTimeはコンポーネント等で保持すべきですが、テスト用にstaticで回します
//	static float animTime = 0.0f;
//	animTime += _deltaTime;
//	double duration = stack->time_end - stack->time_begin;  // ✨ time_end と time_begin に変更
//	double evalTime = fmod(animTime, duration) + stack->time_begin;
//
//	ufbx_anim* anim = stack->anim;
//	std::vector<DirectX::XMMATRIX> worldMatrices(scene->nodes.count);
//
//	// 階層順にノードのワールド行列を計算（ufbxの特性上、1階層のループで親->子が成立します）
//	for (size_t i = 0; i < scene->nodes.count; ++i)
//	{
//		ufbx_node* node = scene->nodes.data[i];
//		ufbx_transform localTRS = ufbx_evaluate_transform(anim, node, evalTime);
//
//		// DirectXMathの型に変換してローカル行列を作成
//		DirectX::XMVECTOR t = DirectX::XMVectorSet((float)localTRS.translation.x, (float)localTRS.translation.y, (float)localTRS.translation.z, 1.0f);
//		DirectX::XMVECTOR r = DirectX::XMVectorSet((float)localTRS.rotation.x, (float)localTRS.rotation.y, (float)localTRS.rotation.z, (float)localTRS.rotation.w);
//		DirectX::XMVECTOR s = DirectX::XMVectorSet((float)localTRS.scale.x, (float)localTRS.scale.y, (float)localTRS.scale.z, 0.0f);
//		DirectX::XMMATRIX localMatrix = DirectX::XMMatrixAffineTransformation(s, DirectX::XMVectorZero(), r, t);
//
//		if (node->parent) {
//			worldMatrices[node->typed_id] = DirectX::XMMatrixMultiply(localMatrix, worldMatrices[node->parent->typed_id]);
//		}
//		else {
//			worldMatrices[node->typed_id] = localMatrix;
//		}
//	}
//
//	// -------------------------------------------------------------
//	// ② 逆バインド行列を合成して、最終的な「スキン行列」を作る
//	// -------------------------------------------------------------
//	if (scene->skin_deformers.count == 0) return;
//	ufbx_skin_deformer* skin = scene->skin_deformers.data[0];
//
//	// シェーダーに送る用の配列（Unlit.hlslのMAX_BONESである256個分の固定長バッファにする）
//	std::vector<cha::Mat4x4> skinningMatrices(256);
//
//	// 安全のためにループ上限をデフォーマーのボーン数か256の小さい方に絞る
//	size_t numBones = std::min(skin->clusters.count, size_t(256));
//
//	for (size_t i = 0; i < numBones; ++i)
//	{
//		ufbx_skin_cluster* cluster = skin->clusters.data[i];
//
//		// ufbxから逆バインド行列を取得してDirectXMathに変換
//		ufbx_matrix m = cluster->geometry_to_bone;
//		DirectX::XMMATRIX invBind = DirectX::XMMatrixSet(
//			(float)m.v[0], (float)m.v[1], (float)m.v[2], 0.0f,
//			(float)m.v[3], (float)m.v[4], (float)m.v[5], 0.0f,
//			(float)m.v[6], (float)m.v[7], (float)m.v[8], 0.0f,
//			(float)m.v[9], (float)m.v[10], (float)m.v[11], 1.0f
//		);
//
//		// スキン行列 ＝ 逆バインド行列 × 現在のボーンのワールド行列
//		DirectX::XMMATRIX currentBoneWorld = worldMatrices[cluster->bone_node->typed_id];
//		DirectX::XMMATRIX finalSkinMatrix = DirectX::XMMatrixMultiply(invBind, currentBoneWorld);
//
//		// ⚠️ HLSLの「列優先」に合わせるため、GPUに送る直前で転置(Transpose)する
//		finalSkinMatrix = DirectX::XMMatrixTranspose(finalSkinMatrix);
//
//		// 自前のMat4x4型などにキャストして配列に格納（中身がfloat16個の連続データならmemcpyでもキャストでも可）
//		DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&skinningMatrices[i]), finalSkinMatrix);
//	}
//
//	// -------------------------------------------------------------
//	// ③ 作成した行列配列を、Modelが持つ専用定数バッファへ転送＆バインド
//	// -------------------------------------------------------------
//	ManagerRegistry::Ref<Dx11Manager>().Ref([&](const ComPtr<ID3D11DeviceContext> _pContext)
//	{
//		D3D11_MAPPED_SUBRESOURCE mappedResource{};
//		HRESULT hr = _pContext->Map(_model.pBoneConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//
//		if (SUCCEEDED(hr))
//		{
//			// 256個分の行列データを一気に転送
//			memcpy(mappedResource.pData, skinningMatrices.data(), sizeof(cha::Mat4x4) * 256);
//			_pContext->Unmap(_model.pBoneConstantBuffer.Get(), 0);
//		}
//
//		// 💡 Unlit.hlsl で指定したレジスタ「b4」に、このモデルのバッファをバインド
//		_pContext->VSSetConstantBuffers(4, 1, _model.pBoneConstantBuffer.GetAddressOf());
//	});
//}
