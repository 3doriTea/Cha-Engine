#pragma once
#include <Base/Base.h>
#include "AudioSystem/AudioPlayer.h"
#include <vector>

struct AudioSystem : SystemObjectBase<AudioSystem>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	void SetBGM(Resource<ResourceType::Audio>& _resource);
	void SetBGMExitLoop();
	void SetBGMStop();
	void SetBGMStart();
	void Play(Resource<ResourceType::Audio>& _resource);

	// 毎フレーム呼ばれる更新処理
	void LateUpdate() override;
	void OnDestroy();

private:
	ComPtr<IXAudio2> pXAudio2{};
	IXAudio2MasteringVoice* pMasterVoice{ nullptr };
	IXAudio2SourceVoice* pBGMSourceVoice{ nullptr };  // BGM用の音声ソース

	std::vector<AudioPlayer> players{}; // 再生中の音声プレイヤー
};
