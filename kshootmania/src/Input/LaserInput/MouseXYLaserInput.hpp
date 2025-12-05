#pragma once
#include "ILaserInputMethod.hpp"

/// @brief マウスX/Y座標式レーザー入力
class MouseXYLaserInput : public ILaserInputMethod
{
private:
	// レーンのインデックス(0=左LASER, 1=右LASER)
	const int32 m_laneIdx;

public:
	explicit MouseXYLaserInput(int32 laneIdx);

	[[nodiscard]]
	double getDeltaCursorX(double deltaTimeSec) override;

	[[nodiscard]]
	bool reconstructionNeeded() const override;
};
