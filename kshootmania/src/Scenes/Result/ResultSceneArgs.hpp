#pragma once
#include "MusicGame/PlayResult.hpp"
#include "Course/CoursePlayState.hpp"

struct ResultSceneArgs
{
	FilePath chartFilePath;

	kson::ChartData chartData;

	MusicGame::PlayResult playResult;

	Optional<CoursePlayState> courseState;
};
