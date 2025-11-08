#pragma once
#include "kson/ChartData.hpp"
#include "MusicGame/GameDefines.hpp"
#include "MusicGame/Camera/CamSystem.hpp"

namespace MusicGame::Camera
{
	class CamPatternSwing
	{
	private:
		const kson::ByPulse<kson::CamPatternInvokeSwing> m_swingEvents;

		std::unordered_set<kson::Pulse> m_alreadyInvokedEventPulses;

		kson::Pulse m_startPulse = kPastPulse;
		kson::RelPulse m_durationRelPulse = kson::RelPulse{ 0 };
		int32 m_direction = 0;
		double m_scale = 1.0;
		uint32 m_repeat = 1;
		uint32 m_decayOrder = 0;

	public:
		explicit CamPatternSwing(const kson::ChartData& chartData);

		void onLaserSlamJudged(kson::Pulse laserSlamPulse, int32 direction, kson::Pulse currentPulse);

		void applyToCamStatus(CamStatus& camStatusRef, kson::Pulse currentPulse) const;

		bool hasEvent(kson::Pulse laserSlamPulse, int32 direction) const;
	};
}
