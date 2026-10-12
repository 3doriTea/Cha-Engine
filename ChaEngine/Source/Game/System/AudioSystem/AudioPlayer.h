#pragma once
#include <Base/Base.h>

/// @brief 単体音声の再生ステータス用
struct AudioPlayer
{
	AudioPlayer(
		IXAudio2SourceVoice* _pSourceVoice,
		Resource<ResourceType::Audio>& _srouce);

	/// @brief この音声プレイヤーが再生中か
	/// @return 再生中 true / false
	inline bool IsPlaying() const
	{
		return timeLeftSec > 0.0f;
	}

	/// @brief 再生する
	/// @param _resource 音声リソース
	void Play(Resource<ResourceType::Audio>& _resource);

	float timeLeftSec{ 0.0f };  // 残りの再生時間(秒)
	IXAudio2SourceVoice* pSourceVoice{ nullptr };
	HashValue formatHash{};
};
