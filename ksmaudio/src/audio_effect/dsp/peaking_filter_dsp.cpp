#include "ksmaudio/audio_effect/dsp/peaking_filter_dsp.hpp"
#include <algorithm>

namespace ksmaudio::AudioEffect
{
	namespace
	{
		// 1�t���[��(1/44100�b)�������v�̐��`�C�[�W���O�̑���(�ő�ω���)
		constexpr float kVEasingSpeed = 0.01f;

		// �t�B���^�K�p�\�ȍŒ���g��
		// (����g����������Ɣg�`�̐U�����ߏ�ɑ傫���Ȃ邽�߂������l��݂��Ă���)
		constexpr float kFreqThresholdMin = 100.0f;

		// TODO: freq�Afreq_max�̒l��ύX�\�ɂ���
		float GetPeakingFilterFreqValue(float v)
		{
			// HSP�łł̌v�Z�����Č���������:
			// https://github.com/m4saka/kshootmania-v1-hsp/blob/08275836547c7792a6d4f59037e56e947f2979c3/src/scene/play/play_audio_effects.hsp#L974

			const float cosV = std::cos(v);
			const float cos2V = cosV * cosV;
			const float baseFreq = 10000.0f - 9950.0f * (cosV * 4 + cos2V * 5) / 9;

			constexpr float kSpeedUpThreshold = 0.8f;
			const float speedUpFreq = (v > kSpeedUpThreshold) ? 3000.0f * (v - kSpeedUpThreshold) / (1.0f - kSpeedUpThreshold) : 0.0f;

			return baseFreq + speedUpFreq;
		}

		// TODO: freq�Afreq_max�̒l����������
		float GetPeakingFilterGainDb(float v, float gainRate)
		{
			// HSP�łł̌v�Z�����Č���������:
			// https://github.com/m4saka/kshootmania-v1-hsp/blob/08275836547c7792a6d4f59037e56e947f2979c3/src/scene/play/play_audio_effects.hsp#L974

			const float baseGain = 14.0f * (std::min)(v, 0.15f) / 0.15f;
			const float midGain = 3.0f * (1.0f - std::abs(v - 0.35f) / 0.35f);
			constexpr float kHiGainCutThreshold = 0.8f;
			const float hiGainCut = (v > kHiGainCutThreshold) ? -3.5f * (v - kHiGainCutThreshold) / (1.0f - kHiGainCutThreshold) : 0.0f;
			return std::clamp(baseGain + midGain + hiGainCut, 0.0f, 18.0f) * gainRate / 0.5f;
		}
	}

	PeakingFilterDSP::PeakingFilterDSP(const DSPCommonInfo& info)
		: m_info(info)
		, m_vEasing(kVEasingSpeed)
	{
	}

	void PeakingFilterDSP::process(float* pData, std::size_t dataSize, bool bypass, const PeakingFilterDSPParams& params)
	{
		if (m_info.isUnsupported)
		{
			return;
		}

		const float fSampleRate = static_cast<float>(m_info.sampleRate);
		const float targetFreq = GetPeakingFilterFreqValue(params.v);
		const float gainDb = GetPeakingFilterGainDb(params.v, params.gainRate);
		const bool isBypassed = bypass || params.mix == 0.0f; // �؂�ւ����̃m�C�Y����̂��߂�bypass��Ԃł��������̂͂���

		assert(dataSize % m_info.numChannels == 0U);
		const std::size_t frameSize = dataSize / m_info.numChannels;
		float freq = GetPeakingFilterFreqValue(m_vEasing.value());
		bool mixSkipped = isBypassed || freq < kFreqThresholdMin; // ����g���ɑ΂��Ă͓K�p���Ȃ�
		for (std::size_t i = 0U; i < frameSize; ++i)
		{
			// �l����Ԃ��ƂŃm�C�Y������Ȃ��悤v�̒l�ɑ΂��Đ��`�̃C�[�W���O������
			const bool vUpdated = m_vEasing.update(params.v);
			if (vUpdated)
			{
				freq = GetPeakingFilterFreqValue(m_vEasing.value());
				mixSkipped = isBypassed || freq < kFreqThresholdMin; // ����g���ɑ΂��Ă͓K�p���Ȃ�
			}

			// �e�`�����l���Ƀt�B���^��K�p
			for (std::size_t ch = 0U; ch < m_info.numChannels; ++ch)
			{
				if (vUpdated)
				{
					m_peakingFilters[ch].setPeakingFilter(freq, params.bandwidth, gainDb, fSampleRate);
				}

				const float wet = m_peakingFilters[ch].process(*pData);
				if (!mixSkipped)
				{
					*pData = std::lerp(*pData, wet, params.mix);
				}
				++pData;
			}
		}
	}
}
