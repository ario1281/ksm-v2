#pragma once
#include <cstddef>

namespace ksmaudio::AudioEffect::detail
{
    class DSPSimpleTriggerHandler
    {
    private:
        std::size_t m_framesSincePrevTrigger = 0U;
        std::ptrdiff_t m_framesUntilTrigger = -1;

    public:
        DSPSimpleTriggerHandler() = default;

        const std::size_t& framesSincePrevTrigger() const
        {
            return m_framesSincePrevTrigger;
        }

        void advance()
        {
            ++m_framesSincePrevTrigger;
            if (m_framesUntilTrigger < 0)
            {
                return;
            }
            if (--m_framesUntilTrigger == 0)
            {
                m_framesSincePrevTrigger = 0U;
            }
        }

        void advanceBatch(std::size_t frames)
        {
            if (m_framesUntilTrigger < 0)
            {
                m_framesSincePrevTrigger += frames;
            }
            else if (m_framesUntilTrigger > static_cast<std::ptrdiff_t>(frames))
            {
                m_framesSincePrevTrigger += frames;
                m_framesUntilTrigger -= frames;
            }
            else
            {
                m_framesSincePrevTrigger = frames - m_framesUntilTrigger;
                m_framesUntilTrigger = -1;
            }
        }

        void setFramesUntilTrigger(float secUntilTrigger, std::size_t sampleRate)
        {
            if (secUntilTrigger >= 0.0f) // Negative value is ignored
            {
                const std::ptrdiff_t newFramesUntilTrigger = static_cast<std::ptrdiff_t>(secUntilTrigger * static_cast<float>(sampleRate));

                // �O���菬�����ꍇ�̂ݔ��f(�g���K�������O�Ɏ��̎��Ԃ����邱�ƂŃg���K�������錻�ۂ�������邽��)
                if (m_framesUntilTrigger < 0 || m_framesUntilTrigger > newFramesUntilTrigger)
                {
                    m_framesUntilTrigger = newFramesUntilTrigger;
                    if (m_framesUntilTrigger == 0)
                    {
                        m_framesSincePrevTrigger = 0U;
                    }
                }
            }
        }
    };
}
