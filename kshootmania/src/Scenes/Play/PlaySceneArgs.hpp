#pragma once
#include "MusicGame/GameDefines.hpp"
#include "MusicGame/PlayOption.hpp"

struct PlaySceneArgs
{
	FilePath chartFilePath;

	MusicGame::PlayOption playOption;

	GaugeType gaugeType = GaugeType::kNormalGauge;
};
