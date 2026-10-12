
#include "AudioSystem.h"
#include <Base/Manager/DeltaTime.h>

void AudioSystem::Start()
{
	XAudio2Create(pXAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	pXAudio2->CreateMasteringVoice(&pMasterVoice);
}

void AudioSystem::Play(Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (!pXAudio2)
	{
		return;
	}

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	HRESULT hResult = pXAudio2->CreateSourceVoice(&pSourceVoice, &audio.waveFormatEx.Format);
	if (FAILED(hResult) || pSourceVoice == nullptr)
	{
		return; // 失敗
	}

	const XAUDIO2_BUFFER BUFFER
	{
		.Flags = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(audio.buffer.size()),
		.pAudioData = audio.buffer.data(),
		.PlayBegin = 0,
		.PlayLength = 0,
		.LoopBegin = 0,
		.LoopLength = 0,
		.LoopCount = 0,
		.pContext = 0,
	};

	hResult = pSourceVoice->SubmitSourceBuffer(&BUFFER);
	if (FAILED(hResult))
	{
		pSourceVoice->DestroyVoice();
		return;
	}

	hResult = pSourceVoice->Start();
	if (FAILED(hResult))
	{
		pSourceVoice->DestroyVoice();
		return;
	}

	// AudioPlayer を作成して管理リストに追加
	players.emplace_back(pSourceVoice, _resource);
	players.back().Play(_resource);
}

void AudioSystem::LateUpdate()
{
	const float delta = Get<DeltaTime>();

	for (size_t i = 0; i < players.size();)
	{
		players[i].timeLeftSec -= delta;
		if (players[i].timeLeftSec <= 0.0f)
		{
			if (players[i].pSourceVoice)
			{
				players[i].pSourceVoice->Stop();
				players[i].pSourceVoice->DestroyVoice();
				players[i].pSourceVoice = nullptr;
			}
			// erase by swapping with last for O(1) removal
			if (i + 1 < players.size())
			{
				players[i] = std::move(players.back());
			}
			players.pop_back();
		}
		else
		{
			++i;
		}
	}
}

void AudioSystem::Update()
{
}

void AudioSystem::SetBGM(Resource<ResourceType::Audio>& _resource)
{
	AudioResource& audio = _resource.Get();

	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
		pBGMSourceVoice->DestroyVoice();
		pBGMSourceVoice = nullptr;
	}

	HRESULT hResult{};

	hResult = pXAudio2->CreateSourceVoice(&pBGMSourceVoice, &audio.waveFormatEx.Format);
	VCU_ASSERT(SUCCEEDED(hResult) && "BGM用SourceVoiceの作成に失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗
	}

	const XAUDIO2_BUFFER BUFFER
	{
		.Flags = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(audio.buffer.size()),
		.pAudioData = audio.buffer.data(),
		.PlayBegin = 0,
		.PlayLength = 0,
		.LoopBegin = 0,
		.LoopLength = 0,
		.LoopCount = XAUDIO2_LOOP_INFINITE,
		.pContext = 0,
	};

	hResult = pBGMSourceVoice->SubmitSourceBuffer(&BUFFER);
	VCU_ASSERT(SUCCEEDED(hResult) && "BGM用バッファ登録に失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗
	}

	hResult = pBGMSourceVoice->Start();
	VCU_ASSERT(SUCCEEDED(hResult) && "BGMの再生に失敗");
	if (FAILED(hResult))
	{
		return;  // 失敗
	}
}

void AudioSystem::SetBGMExitLoop()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->ExitLoop();
	}
}

void AudioSystem::SetBGMStop()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
	}
}

void AudioSystem::SetBGMStart()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Start();
	}
}

void AudioSystem::OnDestroy()
{
	if (pBGMSourceVoice)
	{
		pBGMSourceVoice->Stop();
		pBGMSourceVoice->DestroyVoice();
		pBGMSourceVoice = nullptr;
	}

	if (pMasterVoice)
	{
		pMasterVoice->DestroyVoice();
		pMasterVoice = nullptr;
	}

	// 再生中の全てのソースボイスを停止して破棄
	for (auto& player : players)
	{
		if (player.pSourceVoice)
		{
			player.pSourceVoice->Stop();
			player.pSourceVoice->DestroyVoice();
			player.pSourceVoice = nullptr;
		}
	}
	players.clear();
}
