#include "CoursePlayResult.hpp"

Achievement CoursePlayResult::achievement() const
{
	if (!isCleared)
	{
		return Achievement::kNone;
	}

	bool allPerfect = true;
	bool allFullCombo = true;

	for (const auto& result : results)
	{
		if (result.achievement() < Achievement::kPerfect)
		{
			allPerfect = false;
		}
		if (result.achievement() < Achievement::kFullCombo)
		{
			allFullCombo = false;
		}
	}

	if (allPerfect)
	{
		return Achievement::kPerfect;
	}
	else if (allFullCombo)
	{
		return Achievement::kFullCombo;
	}
	else
	{
		return Achievement::kCleared;
	}
}

int32 CoursePlayResult::gaugePercentForHighScore() const
{
	return achievementRate;
}
