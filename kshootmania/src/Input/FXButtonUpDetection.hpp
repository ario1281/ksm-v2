#pragma once
#include <array>
#include "KeyConfig.hpp"

class FXButtonUpDetection
{
private:
	static constexpr int32 kNumFXButtons = 2;

	std::array<bool, kNumFXButtons> m_otherFXButtonPressed = {};
	std::array<bool, kNumFXButtons> m_excludeKeyPressed = {};

	Array<Input> m_excludeKeys;

	void updateButton(Button targetButton, Button otherFXButton);

public:
	explicit FXButtonUpDetection(const Array<Input>& excludeKeys = {});

	void update();

	bool up(Button targetButton) const;

	void reset();
};
