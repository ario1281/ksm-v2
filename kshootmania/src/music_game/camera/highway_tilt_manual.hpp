#pragma once
#include "kson/chart_data.hpp"

namespace MusicGame::Camera
{
	class HighwayTiltManual
	{
	private:
		double m_radians = 0.0;
		double m_lerpRate = 0.0;

	public:
		HighwayTiltManual() = default;

		void update(const kson::ByPulse<kson::TiltValue>& tilt, kson::Pulse currentPulse);

		double radians() const;

		double lerpRate() const;
	};
}
