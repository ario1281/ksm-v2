#pragma once

class VerticalMarquee : public noco::ComponentBase
{
private:
	double m_startWaitDurationSec;
	double m_endWaitDurationSec;
	double m_speed;

	enum class State
	{
		StartWait,
		Scrolling,
		EndWait,
	};

	State m_state;
	Stopwatch m_stopwatch;

public:
	explicit VerticalMarquee(Duration startWaitDuration = 1s, Duration endWaitDuration = 1s, double speed = 50.0)
		: ComponentBase({})
		, m_startWaitDurationSec(startWaitDuration.count())
		, m_endWaitDurationSec(endWaitDuration.count())
		, m_speed(speed)
		, m_state(State::StartWait)
	{
	}

	void onActivated(const std::shared_ptr<noco::Node>& node) override;

	void update(const std::shared_ptr<noco::Node>& node) override;
};
