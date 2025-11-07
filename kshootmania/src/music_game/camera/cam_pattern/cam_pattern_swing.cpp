#include "cam_pattern_swing.hpp"

namespace MusicGame::Camera
{
	CamPatternSwing::CamPatternSwing(const kson::ChartData& chartData)
		: m_swingEvents(chartData.camera.cam.pattern.laser.slamEvent.swing)
		, m_alreadyInvokedEventPulses(m_swingEvents.size())
	{
	}

	void CamPatternSwing::onLaserSlamJudged(kson::Pulse laserSlamPulse, int32 direction, kson::Pulse currentPulse)
	{
		assert(direction == 1 || direction == -1);

		if (!m_swingEvents.contains(laserSlamPulse))
		{
			return;
		}

		if (m_alreadyInvokedEventPulses.contains(laserSlamPulse))
		{
			return;
		}

		const auto& swingEvent = m_swingEvents.at(laserSlamPulse);

		if (swingEvent.d != direction)
		{
			return;
		}

		m_startPulse = currentPulse;
		m_durationRelPulse = swingEvent.length;
		m_direction = direction;
		m_scale = swingEvent.v.scale;
		m_repeat = swingEvent.v.repeat;
		m_decayOrder = swingEvent.v.decayOrder;

		m_alreadyInvokedEventPulses.insert(laserSlamPulse);
	}

	void CamPatternSwing::applyToCamStatus(CamStatus& camStatusRef, kson::Pulse currentPulse) const
	{
		if (currentPulse < m_startPulse)
		{
			return;
		}

		const kson::RelPulse elapsedPulse = currentPulse - m_startPulse;
		if (elapsedPulse >= m_durationRelPulse)
		{
			return;
		}

		const double rate = static_cast<double>(elapsedPulse) / static_cast<double>(m_durationRelPulse);

		// ばねエフェクトの正弦波の値を計算
		const double sineValue = Sin(Math::Pi * m_repeat * rate);

		// 減衰を適用
		double decayValue = 1.0;
		if (m_decayOrder == 1)
		{
			// 線形
			decayValue = 1.0 - rate;
		}
		else if (m_decayOrder == 2)
		{
			// 2次
			decayValue = Pow(1.0 - rate, 2.0);
		}

		const double swingValue = 10.0 * sineValue * decayValue * (m_scale / 50.0) * m_direction;

		camStatusRef.shiftX += swingValue / 2.0;
	}

	bool CamPatternSwing::hasEvent(kson::Pulse laserSlamPulse, int32 direction) const
	{
		if (!m_swingEvents.contains(laserSlamPulse))
		{
			return false;
		}

		const auto& swingEvent = m_swingEvents.at(laserSlamPulse);
		return swingEvent.d == direction;
	}
}
