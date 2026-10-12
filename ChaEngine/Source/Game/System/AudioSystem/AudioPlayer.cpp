
#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	IXAudio2SourceVoice* _pSourceVoice,
	Resource<ResourceType::Audio>& _srouce) :
	pSourceVoice{ _pSourceVoice },
	formatHash{ _srouce.Get().GetHash() }
{
}

void AudioPlayer::Play(Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();
	VCU_ASSERT(formatHash == audio.GetHash()
		&& "フォーマットが異なる音声リソースはこのプレイヤーで再生できません。");

	if (audio.waveFormatEx.Format.nAvgBytesPerSec == 0)
	{
		timeLeftSec = 0.0f;
	}
	else
	{
		timeLeftSec =
			static_cast<float>(audio.buffer.size())
			/ audio.waveFormatEx.Format.nAvgBytesPerSec;
	}
}
