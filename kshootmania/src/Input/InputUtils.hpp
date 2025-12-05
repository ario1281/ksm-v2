#pragma once
#include <ksmaxis/ksmaxis.hpp>

namespace InputUtils
{
	ksmaxis::DeviceFlags GetLaserInputDeviceFlags();

	void InitKsmaxisForCurrentLaserInput();
}
