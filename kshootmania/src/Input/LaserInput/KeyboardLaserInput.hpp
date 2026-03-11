#pragma once
#include "ILaserInputMethod.hpp"

/// @brief キーボード式レーザー入力
class KeyboardLaserInput : public ILaserInputMethod
{
private:
	const Button m_buttonL;
	const Button m_buttonR;

public:
	KeyboardLaserInput(Button buttonL, Button buttonR);

	[[nodiscard]]
	double getDeltaCursorX(double deltaTimeSec) override;

	[[nodiscard]]
	bool reconstructionNeeded() const override;
};
