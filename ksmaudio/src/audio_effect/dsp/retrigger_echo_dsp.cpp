#include "ksmaudio/audio_effect/dsp/retrigger_echo_dsp.hpp"

namespace ksmaudio::AudioEffect
{
    RetriggerEchoDSP::RetriggerEchoDSP(const DSPCommonInfo& info)
        : m_info(info)
        , m_linearBuffer(
            static_cast<std::size_t>(info.sampleRate) * 10 * info.numChannels, // 10 seconds
            info.numChannels)
    {
    }

    void RetriggerEchoDSP::process(float* pData, std::size_t dataSize, bool bypass, const RetriggerEchoDSPParams& params, bool isParamUpdated)
    {
        assert(dataSize % m_info.numChannels == 0);

        if (isParamUpdated) // secUntilTrigger�����updateTrigger�̒l�̓p�����[�^�X�V��̏�����s���̂ݗL��
        {
            // �g���K�X�V�܂ł̃t���[�������v�Z
            if (params.secUntilTrigger >= 0.0f) // ���̒l�͖���
            {
                m_framesUntilTrigger = static_cast<std::ptrdiff_t>(params.secUntilTrigger * static_cast<float>(m_info.sampleRate));
            }

            // updateTrigger�ɂ��g���K�X�V
            // ("update_trigger"��"off>on"��"off-on"�Ȃǂɂ����ꍇ�̃m�[�c����ɂ��X�V)
            if (params.updateTrigger)
            {
                m_linearBuffer.resetReadWriteCursors();
            }
        }

        const bool active = !bypass && params.mix > 0.0f;
        const std::size_t frameSize = dataSize / m_info.numChannels;
        const std::size_t numLoopFrames = static_cast<std::size_t>(params.waveLength * static_cast<float>(m_info.sampleRate));
        const std::size_t numNonZeroFrames = static_cast<std::size_t>(static_cast<float>(numLoopFrames) * params.rate);
        if (0 <= m_framesUntilTrigger && std::cmp_less(m_framesUntilTrigger, frameSize)) // m_framesUntilTrigger < frameSize
        {
            // ����̏����t���[�����Ƀg���K�X�V�^�C�~���O���܂܂�Ă���ꍇ�A�g���K�X�V�̑O��2�ɕ����ď���

            // �g���K�X�V���O
            const std::size_t formerSize = static_cast<std::size_t>(m_framesUntilTrigger) * m_info.numChannels;
            m_linearBuffer.write(pData, formerSize);
            m_linearBuffer.read(pData, formerSize, numLoopFrames, numNonZeroFrames, params.fadesOut, params.feedbackLevel, active ? params.mix : 0.0f);

            // framesUntilTrigger�ɂ��g���K�X�V
            // ("update_period"��A"update_trigger"��"param_change"��"on"�ɕύX�����ꍇ�̍X�V)
            m_linearBuffer.resetReadWriteCursors();
            m_framesUntilTrigger = -1;

            // �g���K�X�V�����
            const std::size_t latterSize = dataSize - formerSize;
            m_linearBuffer.write(pData + formerSize, latterSize);
            m_linearBuffer.read(pData + formerSize, latterSize, numLoopFrames, numNonZeroFrames, params.fadesOut, params.feedbackLevel, active ? params.mix : 0.0f);
            if (!active)
            {
                m_linearBuffer.resetFadeOutScale();
            }
        }
        else
        {
            // ����̏����t���[�����Ƀg���K�X�V�^�C�~���O���܂܂�Ă��Ȃ��ꍇ�A��x�ɏ���

            m_linearBuffer.write(pData, dataSize);
            m_linearBuffer.read(pData, dataSize, numLoopFrames, numNonZeroFrames, params.fadesOut, params.feedbackLevel, active ? params.mix : 0.0f);
            if (!active)
            {
                m_linearBuffer.resetFadeOutScale();
            }

            // ����g���K�X�V�^�C�~���O�܂ł̎c�莞�Ԃ����炷
            if (std::cmp_greater_equal(m_framesUntilTrigger, frameSize)) // m_framesUntilTrigger >= frameSize
            {
                m_framesUntilTrigger -= frameSize;
            }
        }
    }
}
