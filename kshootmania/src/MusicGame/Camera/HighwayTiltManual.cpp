#include "HighwayTiltManual.hpp"
#include "MusicGame/GameDefines.hpp"

namespace MusicGame::Camera
{
	void HighwayTiltManual::update(const kson::ByPulse<kson::TiltValue>& tilt, kson::Pulse currentPulse)
	{
		const std::optional<double> value = kson::ManualTiltValueAt(tilt, currentPulse);

		m_lerpRate = Clamp(m_lerpRate + Scene::DeltaTime() / 0.5 * (value.has_value() ? 1 : -1), 0.0, 1.0);
		if (value.has_value())
		{
			m_radians = kTiltRadians * value.value();
		}
	}

	double HighwayTiltManual::radians() const
	{
		return m_radians;
	}

	double HighwayTiltManual::lerpRate() const
	{
		return m_lerpRate;
	}
}
