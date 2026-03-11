#pragma once
#include "ILaserInputMethod.hpp"

/// @brief アナログスティックXY循環式レーザー入力
class AnalogStickXYLaserInput : public ILaserInputMethod
{
private:
	// レーンのインデックス(0=左LASER, 1=右LASER)
	const int32 m_laneIdx;

public:
	explicit AnalogStickXYLaserInput(int32 laneIdx);

	[[nodiscard]]
	double getDeltaCursorX(double deltaTimeSec) override;

	[[nodiscard]]
	bool reconstructionNeeded() const override;
};
