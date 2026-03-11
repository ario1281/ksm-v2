#pragma once
#include "MusicGame/GameDefines.hpp"
#include "MusicGame/PlayResult.hpp"

struct CoursePlayResult
{
	int32 avgScore = 0;

	int32 maxCombo = 0;

	int32 achievementRate = 0;

	GaugeType gaugeType = GaugeType::kNormalGauge;

	bool isCleared = false;

	Array<MusicGame::PlayResult> results;

	[[nodiscard]]
	Achievement achievement() const;

	[[nodiscard]]
	int32 gaugePercentForHighScore() const;
};
