#include "ksmaudio/audio_effect/dsp/high_pass_filter_dsp.hpp"

namespace ksmaudio::AudioEffect
{
	namespace
	{
		// 1�t���[��(1/44100�b)�������v�̐��`�C�[�W���O�̑���(�ő�ω���)
		constexpr float kVEasingSpeed = 0.01f;

		// �t�B���^�K�p�\�ȍŒ���g��
		// (����g����������Ɣg�`�̐U�����ߏ�ɑ傫���Ȃ邽�߂������l��݂��Ă���)
		constexpr float kFreqThresholdMin = 200.0f;

		// TODO: freq�Afreq_max�̒l��ύX�\�ɂ���
		float GetHighPassFilterFreqValue(float v)
		{
			// HSP�łł̌v�Z�����Č���������:
			// https://github.com/m4saka/kshootmania-v1-hsp/blob/08275836547c7792a6d4f59037e56e947f2979c3/src/scene/play/play_audio_effects.hsp#L955

			return 4600.0f - 4500.0f * std::cos(v);
		}
	}

	HighPassFilterDSP::HighPassFilterDSP(const DSPCommonInfo& info)
		: m_info(info)
		, m_vEasing(kVEasingSpeed)
	{
	}

	void HighPassFilterDSP::process(float* pData, std::size_t dataSize, bool bypass, const HighPassFilterDSPParams& params, bool isParamUpdated)
	{
		if (m_info.isUnsupported)
		{
			return;
		}

		const float fSampleRate = static_cast<float>(m_info.sampleRate);
		const float targetFreq = GetHighPassFilterFreqValue(params.v);
		const bool isBypassed = bypass || params.mix == 0.0f; // �؂�ւ����̃m�C�Y����̂��߂�bypass��Ԃł��������̂͂���

		assert(dataSize % m_info.numChannels == 0U);
		const std::size_t frameSize = dataSize / m_info.numChannels;
		float freq = GetHighPassFilterFreqValue(m_vEasing.value());
		bool mixSkipped = isBypassed || freq < kFreqThresholdMin; // ����g���ɑ΂��Ă͓K�p���Ȃ�
		for (std::size_t i = 0U; i < frameSize; ++i)
		{
			// �l����Ԃ��ƂŃm�C�Y������Ȃ��悤v�̒l�ɑ΂��Đ��`�̃C�[�W���O������
			const bool vUpdated = m_vEasing.update(params.v);
			if (vUpdated)
			{
				freq = GetHighPassFilterFreqValue(m_vEasing.value());
				mixSkipped = isBypassed || freq < kFreqThresholdMin; // ����g���ɑ΂��Ă͓K�p���Ȃ�
			}

			// �e�`�����l���Ƀt�B���^��K�p
			for (std::size_t ch = 0U; ch < m_info.numChannels; ++ch)
			{
				if (vUpdated)
				{
					m_highPassFilters[ch].setHighPassFilter(freq, params.q, fSampleRate);
				}

				const float wet = m_highPassFilters[ch].process(*pData);
				if (!mixSkipped)
				{
					*pData = std::lerp(*pData, wet, params.mix);
				}
				++pData;
			}
		}
	}
}
