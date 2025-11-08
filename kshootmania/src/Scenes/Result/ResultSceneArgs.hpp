#pragma once
#include "MusicGame/PlayResult.hpp"

struct ResultSceneArgs
{
	FilePath chartFilePath;

	kson::ChartData chartData;

	MusicGame::PlayResult playResult;
};
