#include "VerticalMarquee.hpp"

void VerticalMarquee::onActivated(const std::shared_ptr<noco::Node>& node)
{
	// 事前にレイアウト確定
	node->refreshContainedCanvasLayoutImmediately();

	// スクロール可能範囲の最小値から開始
	const auto [minScroll, maxScroll] = node->validScrollRange();
	const Vec2 currentOffset = node->scrollOffset();
	node->scroll(minScroll - currentOffset);
	m_state = State::StartWait;
	m_stopwatch.restart();
}

void VerticalMarquee::update(const std::shared_ptr<noco::Node>& node)
{
	const double elapsed = m_stopwatch.sF();

	switch (m_state)
	{
	case State::StartWait:
		if (elapsed >= m_startWaitDurationSec)
		{
			m_state = State::Scrolling;
			m_stopwatch.restart();
		}
		break;

	case State::Scrolling:
		{
			node->scroll(Vec2{ 0.0, m_speed * Scene::DeltaTime() });
			const Vec2 currentOffset = node->scrollOffset();

			const auto [minScroll, maxScroll] = node->validScrollRange();

			if (currentOffset.y >= maxScroll.y)
			{
				m_state = State::EndWait;
				m_stopwatch.restart();
			}
		}
		break;

	case State::EndWait:
		if (elapsed >= m_endWaitDurationSec)
		{
			// スクロール可能範囲の最小値に戻す
			const auto [minScroll, maxScroll] = node->validScrollRange();
			const Vec2 currentOffset = node->scrollOffset();
			node->scroll(minScroll - currentOffset);
			m_state = State::StartWait;
			m_stopwatch.restart();
		}
		break;
	}
}
