#pragma once
#include "MusicGame/GameDefines.hpp"
#include "Judgment/ComboStatus.hpp"
#include "PlayOption.hpp"

namespace MusicGame
{
	struct PlayResult
	{
		int32 score = 0;

		int32 maxCombo = 0;

		int32 totalCombo = 0;

		Judgment::ComboStats comboStats;

		PlayOption playOption;

		double gaugePercentage = 0.0;

		double gaugePercentageForGrade = 0.0;

		bool isAborted() const;

		Achievement achievement() const;

		Grade grade() const;

		int32 gaugePercentForHighScore() const;
	};
}
