#pragma once
#include <set>

namespace ksmaudio::AudioEffect::detail
{
    class UpdateTriggerTimeline
    {
	private:
		std::set<float> m_updateTriggerTiming;
		std::set<float>::const_iterator m_updateTriggerTimingCursor;
		float m_secUntilTrigger = -1.0f;

	public:
		explicit UpdateTriggerTimeline(const std::set<float>& updateTriggerTiming)
			: m_updateTriggerTiming(updateTriggerTiming)
			, m_updateTriggerTimingCursor(m_updateTriggerTiming.begin())
		{
		}

		void update(float currentSec)
		{
			if (m_updateTriggerTiming.empty() || m_updateTriggerTimingCursor == m_updateTriggerTiming.end())
			{
				// ���̒l��DSP���Ŗ��������
				m_secUntilTrigger = -1.0f;
				return;
			}

			// �J�[�\�������ݎ��Ԃ܂Ői�߂�
			if (*m_updateTriggerTimingCursor < currentSec)
			{
				do
				{
					++m_updateTriggerTimingCursor;
				} while (m_updateTriggerTimingCursor != m_updateTriggerTiming.end() && *m_updateTriggerTimingCursor < currentSec);

				// ���̒l��DSP���Ŗ��������
				m_secUntilTrigger = -1.0f;
				return;
			}

			m_secUntilTrigger = *m_updateTriggerTimingCursor - currentSec;
		}

		float secUntilTrigger() const
		{
			return m_secUntilTrigger;
		}
    };
}
