#include "InputUtils.hpp"

namespace InputUtils
{
	ksmaxis::DeviceFlags GetLaserInputDeviceFlags()
	{
		const int32 laserInputType = ConfigIni::GetInt(ConfigIni::Key::kLaserInputType, ConfigIni::Value::LaserInputType::kKeyboard);
		switch (laserInputType)
		{
		case ConfigIni::Value::LaserInputType::kMouseXY:
			return ksmaxis::GetRequiredDeviceFlags(ksmaxis::InputMode::kMouse);
		case ConfigIni::Value::LaserInputType::kSlider:
			return ksmaxis::GetRequiredDeviceFlags(ksmaxis::InputMode::kSlider);
		case ConfigIni::Value::LaserInputType::kAnalogStickXY:
			return ksmaxis::GetRequiredDeviceFlags(ksmaxis::InputMode::kAnalogStick);
		default:
			return ksmaxis::DeviceFlags::None;
		}
	}

	void InitKsmaxisForCurrentLaserInput()
	{
		const ksmaxis::DeviceFlags deviceFlags = GetLaserInputDeviceFlags();
		if (deviceFlags != ksmaxis::DeviceFlags::None)
		{
			std::string ksmaxisError;
			std::vector<std::string> ksmaxisWarnings;
#ifdef _WIN32
			const bool ksmaxisSuccess = ksmaxis::Init(deviceFlags, s3d::Platform::Windows::Window::GetHWND(), &ksmaxisError, &ksmaxisWarnings);
#else
			const bool ksmaxisSuccess = ksmaxis::Init(deviceFlags, &ksmaxisError, &ksmaxisWarnings);
#endif
			if (!ksmaxisSuccess)
			{
				Logger << U"[ksm error] ksmaxis::Init() failed: " << Unicode::FromUTF8(ksmaxisError);
			}
			for (const auto& warning : ksmaxisWarnings)
			{
				Logger << U"[ksm warning] ksmaxis: " << Unicode::FromUTF8(warning);
			}
		}
	}
}
