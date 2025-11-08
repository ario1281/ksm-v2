#pragma once
#include "kson/ChartData.hpp"
#include "PlayOption.hpp"

namespace MusicGame
{
	// Off/Hideモードに応じてChartDataのノーツ配列をフィルタリング
	void ApplyPlayModeFilter(kson::ChartData& chartData, const PlayOption& playOption);
}
