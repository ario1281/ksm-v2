#include "KeyConfig.hpp"
#include "Ini/ConfigIni.hpp"

namespace
{
	// キーコンフィグの設定画面や保存時などに配列サイズが固定のほうが都合が良いのでs3d::InputGroupは不使用
	using ConfigSetArray = std::array<Input, KeyConfig::kButtonEnumCount>;
	std::array<ConfigSetArray, KeyConfig::kConfigSetEnumCount> s_configSetArray;

#ifdef __APPLE__
	struct PlatformKeyState
	{
		bool previousPressed = false;
		bool currentPressed = false;

		void update(bool pressed)
		{
			previousPressed = currentPressed;
			currentPressed = pressed;
		}

		bool pressed() const { return currentPressed; }
		bool down() const { return currentPressed && !previousPressed; }
		bool up() const { return !currentPressed && previousPressed; }
	};

	std::array<PlatformKeyState, kPlatformKeys.size()> s_platformKeyStates;
#endif

	constexpr std::array<InputDeviceType, KeyConfig::kConfigSetEnumCount> kConfigSetDeviceTypes = {
		InputDeviceType::Keyboard,
		InputDeviceType::Keyboard,
		InputDeviceType::Gamepad,
		InputDeviceType::Gamepad,
	};

	constexpr std::array<StringView, KeyConfig::kConfigSetEnumCount> kConfigSetNames = {
		U"Keyboard 1",
		U"Keyboard 2",
		U"Gamepad 1",
		U"Gamepad 2",
	};

	StringView ConfigSetToConfigIniKey(KeyConfig::ConfigSet configSet)
	{
		switch (configSet)
		{
		case KeyConfig::kKeyboard1:
			return ConfigIni::Key::kKeyConfigKeyboard1;
		case KeyConfig::kKeyboard2:
			return ConfigIni::Key::kKeyConfigKeyboard2;
		case KeyConfig::kGamepad1:
			return ConfigIni::Key::kKeyConfigGamepad1;
		case KeyConfig::kGamepad2:
			return ConfigIni::Key::kKeyConfigGamepad2;
		default:
			throw Error(U"ConfigSetToConfigIniKey(): Invalid configSet");
		}
	}

	void RevertUnconfigurableKeyConfigs()
	{
		// Keyboard 1の場合、ユーザーによって変更できない固定のキーコンフィグがあるので上書き
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kStart] = KeyEnter;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kBack] = KeyEscape;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kAutoPlay] = KeyF11;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kUp] = KeyUp;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kDown] = KeyDown;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kLeft] = KeyLeft;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kRight] = KeyRight;
		s_configSetArray[KeyConfig::kKeyboard1][KeyConfig::kBackspace] = KeyBackspace;
	}

	bool IsBT3PlusStartPressed()
	{
		if (!ConfigIni::GetBool(ConfigIni::Key::kUse3BTsPlusStartAsBack, true))
		{
			return false;
		}

		bool startPressed = false;
		for (const auto& configSet : s_configSetArray)
		{
			if (configSet[KeyConfig::kStart].pressed())
			{
				startPressed = true;
				break;
			}
		}
		if (!startPressed)
		{
			return false;
		}

		int32 btPressedCount = 0;
		for (int32 btIdx = KeyConfig::kBT_A; btIdx <= KeyConfig::kBT_D; ++btIdx)
		{
			for (const auto& configSet : s_configSetArray)
			{
				if (configSet[btIdx].pressed())
				{
					++btPressedCount;
					break;
				}
			}
		}

		return btPressedCount >= 3;
	}

	bool IsBT3PlusStartDown()
	{
		if (!IsBT3PlusStartPressed())
		{
			return false;
		}

		for (const auto& configSet : s_configSetArray)
		{
			if (configSet[KeyConfig::kStart].down())
			{
				return true;
			}
		}

		for (int32 btIdx = KeyConfig::kBT_A; btIdx <= KeyConfig::kBT_D; ++btIdx)
		{
			for (const auto& configSet : s_configSetArray)
			{
				if (configSet[btIdx].down())
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsBT3PlusStartUp()
	{
		if (IsBT3PlusStartPressed())
		{
			return false;
		}

		for (const auto& configSet : s_configSetArray)
		{
			if (configSet[KeyConfig::kStart].up())
			{
				return true;
			}
		}

		for (int32 btIdx = KeyConfig::kBT_A; btIdx <= KeyConfig::kBT_D; ++btIdx)
		{
			for (const auto& configSet : s_configSetArray)
			{
				if (configSet[btIdx].up())
				{
					return true;
				}
			}
		}

		return false;
	}
}

void KeyConfig::SetConfigValueByCommaSeparated(ConfigSet targetConfigSet, StringView configValue)
{
	Array<String> values = String(configValue).split(U',');

	if (targetConfigSet < 0 || kConfigSetEnumCount <= targetConfigSet)
	{
		Logger << U"[ksm warning] Invalid key config target '{}'!"_fmt(static_cast<std::underlying_type_t<ConfigSet>>(targetConfigSet));
		return;
	}

	if (values.size() != kConfigurableButtonEnumCount)
	{
		Logger << U"[ksm warning] Key configuration ({}) is ignored because value count does not match! (expected:{}, actual:{})"_fmt(kConfigSetNames[targetConfigSet], static_cast<int32>(kConfigurableButtonEnumCount), values.size());
		values = String(kDefaultConfigValues[targetConfigSet]).split(U',');
	}

	Array<int32> intValues;
	try
	{
		intValues = values.map([](const String& str) { return Parse<int32>(str); });
	}
	catch (const ParseError&)
	{
		Print << U"Warning: Key configuration ({}) is ignored due to parse error!"_fmt(kConfigSetNames[targetConfigSet]);
		try
		{
			intValues = String(kDefaultConfigValues[targetConfigSet]).split(U',').map([](const String& str) { return Parse<int32>(str); });
		}
		catch (const ParseError&)
		{
			throw Error(U"KeyConfig::SetConfigValue(): Could not parse KeyConfig::kDefaultConfigValues!");
		}
	}

	for (int32 i = 0; i < kConfigurableButtonEnumCount; ++i)
	{
		const int32 code = intValues[i];
		if (code >= 0)
		{
			s_configSetArray[targetConfigSet][i] = Input(kConfigSetDeviceTypes[targetConfigSet], static_cast<uint8>(code));
		}
		else
		{
			s_configSetArray[targetConfigSet][i] = Input();
		}
	}

	RevertUnconfigurableKeyConfigs();
}

void KeyConfig::SetConfigValue(ConfigSet targetConfigSet, ConfigurableButton button, const Input& input)
{
	if (targetConfigSet < 0 || kConfigSetEnumCount <= targetConfigSet)
	{
		throw Error(U"Warning: Invalid key config target '{}'!"_fmt(static_cast<std::underlying_type_t<ConfigSet>>(targetConfigSet)));
	}

	if (button < 0 || kConfigurableButtonEnumCount <= button)
	{
		throw Error(U"Warning: Invalid key config button '{}'!"_fmt(static_cast<std::underlying_type_t<ConfigurableButton>>(button)));
	}

	s_configSetArray[targetConfigSet][button] = input;

	RevertUnconfigurableKeyConfigs();
}

const Input& KeyConfig::GetConfigValue(ConfigSet targetConfigSet, ConfigurableButton button)
{
	if (targetConfigSet < 0 || kConfigSetEnumCount <= targetConfigSet)
	{
		throw Error(U"Warning: Invalid key config target '{}'!"_fmt(static_cast<std::underlying_type_t<ConfigSet>>(targetConfigSet)));
	}

	if (button < 0 || kConfigurableButtonEnumCount <= button)
	{
		throw Error(U"Warning: Invalid key config button '{}'!"_fmt(static_cast<std::underlying_type_t<ConfigurableButton>>(button)));
	}

	return s_configSetArray[targetConfigSet][button];
}

#ifdef __APPLE__
void KeyConfig::UpdatePlatformKeyboard()
{
	for (size_t i = 0; i < kPlatformKeys.size(); ++i)
	{
		const bool pressed = KSMPlatformMacOS_IsKeyPressed(kPlatformKeys[i].nativeCode);
		s_platformKeyStates[i].update(pressed);
	}
}
#endif

void KeyConfig::SaveToConfigIni()
{
	for (int32 configSetIdx = 0; configSetIdx < kConfigSetEnumCount; ++configSetIdx)
	{
		// キーコード一覧をカンマ区切りの文字列に変換
		String configValue;
		for (int32 i = 0; i < kConfigurableButtonEnumCount; ++i)
		{
			int32 code;
			if (s_configSetArray[configSetIdx][i].deviceType() == InputDeviceType::Undefined)
			{
				code = -1;
			}
			else
			{
				code = static_cast<int32>(s_configSetArray[configSetIdx][i].code());
			}
			configValue += Format(code);
			configValue += U",";
		}
		configValue.pop_back();

		// 保存
		const auto configSet = static_cast<ConfigSet>(configSetIdx);
		const StringView configIniKey = ConfigSetToConfigIniKey(configSet);
		ConfigIni::SetString(configIniKey, configValue);
	}
}

bool KeyConfig::Pressed(Button button)
{
	if (button == KeyConfig::kUnspecifiedButton)
	{
		return false;
	}

	for (const auto& configSet : s_configSetArray)
	{
		const auto& input = configSet[button];
		if (input.deviceType() == InputDeviceType::Keyboard)
		{
#ifdef __APPLE__
			bool isPlatformKey = false;
			for (size_t i = 0; i < kPlatformKeys.size(); ++i)
			{
				if (input.code() == (kPlatformKeys[i].code - kPlatformKeyCodeOffset))
				{
					if (s_platformKeyStates[i].pressed())
					{
						return true;
					}
					isPlatformKey = true;
					break;
				}
			}
			if (!isPlatformKey && input.pressed())
			{
				return true;
			}
#else
			if (input.pressed())
			{
				return true;
			}
#endif
		}
		else if (input.pressed())
		{
			return true;
		}
	}

	// FXの場合はLR両押しキーの状態も反映
	if (button == kFX_L || button == kFX_R)
	{
		for (const auto& configSet : s_configSetArray)
		{
			if (configSet[kFX_LR].pressed())
			{
				return true;
			}
		}
	}

	if (button == kBack)
	{
		if (IsBT3PlusStartPressed())
		{
			return true;
		}
	}

	return false;
}

Optional<KeyConfig::Button> KeyConfig::LastPressedLaserButton(Button button1, Button button2)
{
	assert(button1 == kLeftLaserL || button1 == kLeftLaserR || button1 == kRightLaserL || button1 == kRightLaserR);
	assert(button2 == kLeftLaserL || button2 == kLeftLaserR || button2 == kRightLaserL || button2 == kRightLaserR);

	Optional<Button> lastButton = none;
	Duration minDuration = Duration::zero();
	for (const auto& configSet : s_configSetArray)
	{
		for (Button button : { button1, button2 })
		{
			if (configSet[button].pressed())
			{
				const Duration duration = configSet[button].pressedDuration();
				if (!lastButton.has_value() || duration < minDuration)
				{
					minDuration = duration;
					lastButton = button;
				}
			}
		}
	}

	return lastButton;
}

bool KeyConfig::Down(Button button)
{
	if (button == KeyConfig::kUnspecifiedButton)
	{
		return false;
	}

	for (const auto& configSet : s_configSetArray)
	{
		const auto& input = configSet[button];
		if (input.deviceType() == InputDeviceType::Keyboard)
		{
#ifdef __APPLE__
			bool isPlatformKey = false;
			for (size_t i = 0; i < kPlatformKeys.size(); ++i)
			{
				if (input.code() == (kPlatformKeys[i].code - kPlatformKeyCodeOffset))
				{
					if (s_platformKeyStates[i].down())
					{
						return true;
					}
					isPlatformKey = true;
					break;
				}
			}
			if (!isPlatformKey && input.down())
			{
				return true;
			}
#else
			if (input.down())
			{
				return true;
			}
#endif
		}
		else if (input.down())
		{
			return true;
		}
	}

	// FXの場合はLR両押しキーの状態も反映
	if (button == kFX_L || button == kFX_R)
	{
		for (const auto& configSet : s_configSetArray)
		{
			if (configSet[kFX_LR].down())
			{
				return true;
			}
		}
	}

	if (button == kBack)
	{
		if (IsBT3PlusStartDown())
		{
			return true;
		}
	}

	return false;
}

void KeyConfig::ClearInput(Button button)
{
	if (button == KeyConfig::kUnspecifiedButton)
	{
		return;
	}

	for (auto& configSet : s_configSetArray)
	{
		configSet[button].clearInput();
	}

	// FXの場合はLR両押しキーの状態もクリア
	if (button == kFX_L || button == kFX_R)
	{
		for (auto& configSet : s_configSetArray)
		{
			configSet[kFX_LR].clearInput();
		}
	}
}

Co::Task<void> KeyConfig::WaitUntilDown(Button button)
{
	// 前シーンの入力で多重に反応しないよう、入力をクリア
	ClearInput(button);

	return Co::WaitUntil([button]() { return Down(button); });
}

bool KeyConfig::Up(Button button)
{
	if (button == KeyConfig::kUnspecifiedButton)
	{
		return false;
	}

	// あるボタンを離したと判定するには、そのボタンに対応する全キーを離している必要がある
	if (Pressed(button))
	{
		return false;
	}

	for (const auto& configSet : s_configSetArray)
	{
		const auto& input = configSet[button];
		if (input.deviceType() == InputDeviceType::Keyboard)
		{
#ifdef __APPLE__
			bool isPlatformKey = false;
			for (size_t i = 0; i < kPlatformKeys.size(); ++i)
			{
				if (input.code() == (kPlatformKeys[i].code - kPlatformKeyCodeOffset))
				{
					if (s_platformKeyStates[i].up())
					{
						return true;
					}
					isPlatformKey = true;
					break;
				}
			}
			if (!isPlatformKey && input.up())
			{
				return true;
			}
#else
			if (input.up())
			{
				return true;
			}
#endif
		}
		else if (input.up())
		{
			return true;
		}
	}

	if (button == kBack)
	{
		if (IsBT3PlusStartUp())
		{
			return true;
		}
	}

	return false;
}
