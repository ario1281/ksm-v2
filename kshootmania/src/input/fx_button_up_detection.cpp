#include "fx_button_up_detection.hpp"

namespace
{
	int32 GetFXButtonIndex(KeyConfig::Button button)
	{
		return button - KeyConfig::kFX_L;
	}
}

FXButtonUpDetection::FXButtonUpDetection(const Array<Input>& excludeKeys)
	: m_excludeKeys(excludeKeys)
{
}

void FXButtonUpDetection::updateButton(KeyConfig::Button targetButton, KeyConfig::Button otherFXButton)
{
	const int32 targetIdx = GetFXButtonIndex(targetButton);

	// 対象ボタンが押された瞬間、もう片方のFXボタンと除外キーの状態を記録
	if (KeyConfig::Down(targetButton))
	{
		// 記録をリセットしてから新しい状態を記録
		m_otherFXButtonPressed[targetIdx] = KeyConfig::Pressed(otherFXButton);

		m_excludeKeyPressed[targetIdx] = false;
		for (const auto& key : m_excludeKeys)
		{
			if (key.pressed())
			{
				m_excludeKeyPressed[targetIdx] = true;
				break;
			}
		}
	}

	// 対象ボタンを押している間にもう片方のFXボタンまたは除外キーが押されたら記録
	if (KeyConfig::Pressed(targetButton))
	{
		if (KeyConfig::Pressed(otherFXButton))
		{
			m_otherFXButtonPressed[targetIdx] = true;
		}

		for (const auto& key : m_excludeKeys)
		{
			if (key.pressed())
			{
				m_excludeKeyPressed[targetIdx] = true;
				break;
			}
		}
	}
}

void FXButtonUpDetection::update()
{
	updateButton(KeyConfig::kFX_L, KeyConfig::kFX_R);
	updateButton(KeyConfig::kFX_R, KeyConfig::kFX_L);
}

bool FXButtonUpDetection::up(KeyConfig::Button targetButton) const
{
	const int32 targetIdx = GetFXButtonIndex(targetButton);
	return KeyConfig::Up(targetButton) && !m_otherFXButtonPressed[targetIdx] && !m_excludeKeyPressed[targetIdx];
}

void FXButtonUpDetection::reset()
{
	m_otherFXButtonPressed.fill(false);
	m_excludeKeyPressed.fill(false);
}
