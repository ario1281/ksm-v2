#include "LaserCursorInputDevice.hpp"
#include "Input/KeyConfig.hpp"

namespace
{
	// カーソル移動のしきい値
	constexpr double kThresholdFirst = 0.5;
	constexpr double kThresholdSecond = 0.7;
	constexpr double kThreshold = 0.4;
	constexpr double kThresholdOver10 = 0.2;

	// 入力がない状態が続いた場合にリセットするまでの時間
	constexpr Duration kIdleResetTime = 0.3s;

	// 入力の有無を判定する最小しきい値
	constexpr double kInputDetectionThreshold = 0.001;

	double GetThreshold(int32 count)
	{
		// 移動回数に応じてしきい値を変える
		// (単発でのカーソル移動をしやすくするため)
		if (count == 0)
		{
			return kThresholdFirst;
		}
		if (count == 1)
		{
			return kThresholdSecond;
		}
		if (count > 10)
		{
			return kThresholdOver10;
		}
		return kThreshold;
	}

	bool CanMoveMultipleSteps(int32 count)
	{
		// 一定以上動くまでは1フレームに複数ステップ移動しない
		return count >= 2;
	}

	bool IsDirectionFlipped(double prev, double current)
	{
		return Sign(prev) != Sign(current) && prev != 0.0 && current != 0.0;
	}
}

LaserCursorInputDevice::LaserCursorInputDevice(int32 laneIdx)
	: m_laneIdx(laneIdx)
{
}

LaserCursorInputDevice::~LaserCursorInputDevice() = default;

void LaserCursorInputDevice::update()
{
	m_deltaCursor = 0;

	double prevAccumulatedDelta = m_accumulatedDelta;

	// レーザーカーソル移動量を取得して加算
	const double deltaCursorX = KeyConfig::LaserDeltaCursorX(m_laneIdx, Scene::DeltaTime());
	m_accumulatedDelta += deltaCursorX;

	if (IsDirectionFlipped(prevAccumulatedDelta, m_accumulatedDelta))
	{
		// 累積移動量の符号が変わったら移動回数をリセット
		m_movementCount = 0;
	}

	if (Abs(deltaCursorX) > kInputDetectionThreshold)
	{
		// 入力があれば入力なし時間をリセット
		m_idleStopwatch.restart();
	}
	else if (m_idleStopwatch.elapsed() > kIdleResetTime)
	{
		// 一定時間入力がなければ移動回数と累積値をリセット
		m_movementCount = 0;
		m_accumulatedDelta = 0.0;
	}

	// 累積カーソル移動量がしきい値を超えたらカーソル移動
	const double threshold = GetThreshold(m_movementCount);
	if (Abs(m_accumulatedDelta) >= threshold)
	{
		if (CanMoveMultipleSteps(m_movementCount))
		{
			// 一定以上動いたら、1フレーム内に複数ステップまとめて移動可能
			const int32 steps = static_cast<int32>(m_accumulatedDelta / threshold);
			m_deltaCursor = steps;
			m_accumulatedDelta -= steps * threshold;
			m_movementCount += Abs(steps);
		}
		else
		{
			// 一定以上動くまでは1フレームに1回だけ移動
			m_deltaCursor = m_accumulatedDelta < 0.0 ? -1 : 1;
			m_accumulatedDelta = 0.0;
			m_movementCount++;
		}
	}
}

int32 LaserCursorInputDevice::deltaCursor() const
{
	return m_deltaCursor;
}
