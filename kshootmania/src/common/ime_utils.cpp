#include "ime_utils.hpp"

#ifdef _WIN32
#include <Windows.h>

void IMEUtils::DetachIMEContext()
{
	const auto hWnd = static_cast<HWND>(Platform::Windows::Window::GetHWND());
	if (hWnd != NULL)
	{
		ImmAssociateContext(hWnd, NULL);
	}
}
#elif defined(__APPLE__)
#include <ksmplatform_macos/input_method.h>

void IMEUtils::DetachIMEContext()
{
	KSMPlatformMacOS_DisableIME();
}
#endif
