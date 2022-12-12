#pragma once
#include <algorithm>
#include "ring_buffer.hpp"

namespace ksmaudio::AudioEffect::detail
{
    class TimeModulator
    {
    private:
        RingBuffer<float> m_ringBuffer;
        float m_delaySample = 0.0f;
        const float m_maxDelayFrames;

    public:
        TimeModulator(std::size_t size, std::size_t numChannels)
            : m_ringBuffer(size, numChannels)
            , m_maxDelayFrames(static_cast<float>(m_ringBuffer.numFrames()))
        {
        }

        void update(float playSpeed)
        {
            // �Đ����x�����ƂɃf�B���C�^�C�����X�V
            if (playSpeed != 1.0f)
            {
                m_delaySample = std::clamp(m_delaySample + 1.0f - playSpeed, 0.0f, m_maxDelayFrames);
            }
        }

        // 1�`�����l���ɂ��Ēl��ǂݍ���/��������
        // (update�܂���resetDelayTime���Ă񂾌�Ŏg�p���邱��)
        float readWrite(float value, std::size_t channel)
        {
            // �o�b�t�@�ɃT���v�����L�^
            m_ringBuffer.write(value, channel);

            // �f�B���C�T���v�����o�b�t�@����擾
            float wet;
            if (m_delaySample > 0.0f)
            {
                wet = m_ringBuffer.lerpedDelay(m_delaySample, channel);
            }
            else
            {
                wet = value;
            }

            return wet;
        }

        void advanceCursor()
        {
            m_ringBuffer.advanceCursor();
        }

        void advanceCursor(std::size_t frameCount)
        {
            m_ringBuffer.advanceCursor(frameCount);
        }

        // �S�`�����l���ɂ��Ă܂Ƃ߂Ēl����������
        // (update�܂���resetDelayTime���Ă񂾌�Ŏg�p���邱��)
        void writeAndAdvanceCursor(const float* pData, std::size_t size)
        {
            assert(size % m_ringBuffer.numChannels() == 0U);
            m_ringBuffer.write(pData, size);
            m_ringBuffer.advanceCursor(size / m_ringBuffer.numChannels());
        }

        // �f�B���C�^�C�������Z�b�g����0�ɂ���
        // (�Đ����x��LFO�ŗh�炷�ꍇ�ɁA�������Ƃɂ�����ĂԂƕ��������_���̌v�Z�덷�̒~�ς�h�~�ł���)
        void resetDelayTime()
        {
            m_delaySample = 0.0f;
        }
    };
}
