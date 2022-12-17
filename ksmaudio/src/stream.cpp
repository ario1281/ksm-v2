#include "ksmaudio/stream.hpp"
#include "ksmaudio/ksmaudio.hpp"

namespace
{
	// �R���v���b�T�[�̃p�����[�^�l
	// (�ݒ�l���ɒ[�Ȃ̂ŉ��P���������ǂ����A�ЂƂ܂�HSP�łƓ����l�ɂ��Ă���)
	// HSP��: https://github.com/m4saka/kshootmania-v1-hsp/blob/461901f1e925cb8cb474fd02726084cfca9ec3d4/kshootmania.hsp#L792
	constexpr BASS_BFX_COMPRESSOR2 kCompressorFXParams
	{
		.fGain = 18.0f,
		.fThreshold = -24.0f,
		.fRatio = 1000.0f,
		.fAttack = 0.01f,
		.fRelease = 0.01f,
		.lChannel = BASS_BFX_CHANALL,
	};

	// �R���v���b�T�[�̗D�揇��
	constexpr int kCompressorFXPriority = 0;

	HSTREAM LoadStream(const std::string& filePath)
	{
		return BASS_StreamCreateFile(FALSE, filePath.c_str(), 0, 0, BASS_STREAM_PRESCAN);
	}

	BASS_CHANNELINFO GetChannelInfo(HSTREAM hStream)
	{
		BASS_CHANNELINFO info;
		BASS_ChannelGetInfo(hStream, &info);
		return info;
	}

	void ProcessAudioEffect(HDSP handle, DWORD channel, void* buffer, DWORD length, void* user)
	{
		const auto pAudioEffect = reinterpret_cast<ksmaudio::AudioEffect::IAudioEffect*>(user);
		const auto pData = reinterpret_cast<float*>(buffer);
		pAudioEffect->process(pData, length / sizeof(float));
	}
}

namespace ksmaudio
{
	Stream::Stream(const std::string& filePath, double volume, bool enableCompressor)
		: m_hStream(LoadStream(filePath))
		, m_info(GetChannelInfo(m_hStream))
	{
		// ���ʂ�ݒ�
		BASS_ChannelSetAttribute(m_hStream, BASS_ATTRIB_VOL, static_cast<float>(volume));

		// �R���v���b�T�[��K�p
		if (enableCompressor)
		{
			const HFX hCompressorFX = BASS_ChannelSetFX(m_hStream, BASS_FX_BFX_COMPRESSOR2, kCompressorFXPriority);
			BASS_FXSetParameters(hCompressorFX, &kCompressorFXParams);
		}
	}

	Stream::~Stream()
	{
		BASS_StreamFree(m_hStream);
	}

	void Stream::play() const
	{
		BASS_ChannelPlay(m_hStream, FALSE);
	}

	void Stream::pause() const
	{
		BASS_ChannelPause(m_hStream);
	}

	void Stream::stop() const
	{
		BASS_ChannelStop(m_hStream);
	}

	void Stream::updateManually() const
	{
		BASS_ChannelUpdate(m_hStream, 0);
	}

	double Stream::posSec() const
	{
		return BASS_ChannelBytes2Seconds(m_hStream, BASS_ChannelGetPosition(m_hStream, BASS_POS_BYTE));
	}

	void Stream::seekPosSec(double timeSec) const
	{
		BASS_ChannelSetPosition(m_hStream, BASS_ChannelSeconds2Bytes(m_hStream, timeSec), 0);
	}

	double Stream::durationSec() const
	{
		return BASS_ChannelBytes2Seconds(m_hStream, BASS_ChannelGetLength(m_hStream, BASS_POS_BYTE));
	}

	HDSP Stream::addAudioEffect(AudioEffect::IAudioEffect* pAudioEffect, int priority) const
	{
		return BASS_ChannelSetDSP(m_hStream, ProcessAudioEffect, pAudioEffect, priority);
	}

	void Stream::removeAudioEffect(HDSP hDSP) const
	{
		BASS_ChannelRemoveDSP(m_hStream, hDSP);
	}

	std::size_t Stream::sampleRate() const
	{
		return static_cast<std::size_t>(m_info.freq);
	}

	std::size_t Stream::numChannels() const
	{
		return static_cast<std::size_t>(m_info.chans);
	}

	double Stream::latencySec() const
	{
		/*DWORD playbuf = BASS_ChannelGetData(m_hStream, NULL, BASS_DATA_AVAILABLE);
		if (playbuf != (DWORD)-1)
		{
			return BASS_ChannelBytes2Seconds(m_hStream, playbuf);
		}*/

		// ��L�ł����o�b�t�@�T�C�Y�Ɠ����ɂȂ邪�ϓ�����̂ŁA���̂܂܃o�b�t�@�T�C�Y��Ԃ������������G�t�F�N�g�̃^�C�~���O�v�Z�����肷��
		return kBufferSizeMs / 1000.0f;
	}
}
