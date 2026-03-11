#include "KeyboardLaserInput.hpp"
#include "Input/KeyConfig.hpp"

namespace
{
	constexpr double kLaserKeyboardCursorXPerSec = 2.0;
}

KeyboardLaserInput::KeyboardLaserInput(Button buttonL, Button buttonR)
	: m_buttonL(buttonL)
	, m_buttonR(buttonR)
{
}

double KeyboardLaserInput::getDeltaCursorX(double deltaTimeSec)
{
	// 左向きキーと右向きキーを同時に押している場合、最後に押した方を優先する
	const Optional<Button> lastPressedButton = KeyConfig::LastPressedLaserButton(m_buttonL, m_buttonR);
	if (lastPressedButton.has_value())
	{
		const int32 direction = lastPressedButton == m_buttonL ? -1 : 1;
		return kLaserKeyboardCursorXPerSec * deltaTimeSec * direction;
	}
	else
	{
		return 0.0;
	}
}

bool KeyboardLaserInput::reconstructionNeeded() const
{
	const int32 laserInputType = ConfigIni::GetInt(ConfigIni::Key::kLaserInputType, ConfigIni::Value::LaserInputType::kKeyboard);
	return laserInputType != ConfigIni::Value::LaserInputType::kKeyboard;
}
