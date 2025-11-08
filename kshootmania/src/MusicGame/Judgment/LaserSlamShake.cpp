#include "LaserSlamShake.hpp"

namespace MusicGame::Judgment
{
	double LaserSlamShake::shiftX(double currentTimeSec) const
	{
		constexpr double kShakeDurationSec = 0.15;
		constexpr double kShakeMaxShiftX = 17.0 * 50.0 / 150.0 / 2.0;
		const double diffTimeSec = currentTimeSec - m_startSec;
		if (diffTimeSec < kShakeDurationSec)
		{
			const double rate = diffTimeSec / kShakeDurationSec;
			const double invRate = 1.0 - rate;
			return Sin(Math::Pi * rate) * invRate * invRate * m_direction * kShakeMaxShiftX;
		}
		else
		{
			return 0.0;
		}
	}

	void LaserSlamShake::onLaserSlamJudged(double prevTimeSec, int32 direction)
	{
		m_startSec = prevTimeSec;
		m_direction = direction;
	}

	void LaserSlamShake::applyToCamStatus(Camera::CamStatus& camStatusRef, double currentTimeSec) const
	{
		camStatusRef.shiftX += shiftX(currentTimeSec);
	}
}
