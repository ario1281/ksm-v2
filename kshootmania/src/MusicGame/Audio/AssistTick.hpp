#pragma once
#include "MusicGame/GameDefines.hpp"
#include "kson/ChartData.hpp"
#include "kson/Util/TimingUtils.hpp"
#include "ksmaudio/ksmaudio.hpp"

namespace MusicGame::Audio
{
	class AssistTick
	{
	private:
		AssistTickMode m_mode;
		ksmaudio::Sample m_btTickSound;
		ksmaudio::Sample m_fxTickSound;

		std::array<kson::Pulse, kson::kNumBTLanesSZ> m_btPlayedPulses = { kPastPulse, kPastPulse, kPastPulse, kPastPulse };
		std::array<kson::Pulse, kson::kNumFXLanesSZ> m_fxPlayedPulses = { kPastPulse, kPastPulse };

	public:
		explicit AssistTick(AssistTickMode mode);

		void update(const kson::ChartData& chartData, const kson::TimingCache& timingCache, double currentTimeSec);
	};
}
