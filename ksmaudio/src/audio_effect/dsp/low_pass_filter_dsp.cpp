#include "ksmaudio/audio_effect/dsp/low_pass_filter_dsp.hpp"

namespace ksmaudio::AudioEffect
{
	namespace
	{
		constexpr float kFreqSpeed = 5.0f;
	}

	LowPassFilterDSP::LowPassFilterDSP(const DSPCommonInfo& info)
		: m_info(info)
		, m_freqEasing(kFreqSpeed)
	{
	}

	void LowPassFilterDSP::process(float* pData, std::size_t dataSize, bool bypass, const LowPassFilterDSPParams& params)
	{
		if (m_info.isUnsupported)
		{
			return;
		}

		const float fSampleRate = static_cast<float>(m_info.sampleRate);

		const bool isBypassed = bypass || params.mix == 0.0f; // �؂�ւ����̃m�C�Y����̂��߂�bypass��Ԃł��������̂͂���
		const bool isSkipped = std::abs(params.v - m_prevV) > 0.2f || params.freq > 14800.0f; // �m�C�Y����̂��߁A���p���Œl����񂾏u�Ԃ�A�����g���ɑ΂��Ă͓K�p���Ȃ�
		// TODO: isSkipped�͒��p�����莞�Ԃ��ǂ����Ŕ��肷��悤�ɂ���

		assert(dataSize % m_info.numChannels == 0);
		const std::size_t frameSize = dataSize / m_info.numChannels;
		for (std::size_t i = 0U; i < frameSize; ++i)
		{
			const bool freqUpdated = m_freqEasing.update(params.freq);

			for (std::size_t ch = 0U; ch < m_info.numChannels; ++ch)
			{
				if (freqUpdated)
				{
					m_lowPassFilters[ch].setLowPassFilter(m_freqEasing.value(), params.q, fSampleRate);
				}

				const float wet = m_lowPassFilters[ch].process(*pData);
				if (!isBypassed && !isSkipped)
				{
					*pData = std::lerp(*pData, wet, params.mix);
				}
				++pData;
			}
		}

		m_prevV = params.v;
	}
}
