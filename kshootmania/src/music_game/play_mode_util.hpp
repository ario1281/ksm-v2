#pragma once
#include "kson/chart_data.hpp"
#include "play_option.hpp"

namespace MusicGame
{
	// Off/Hideモードに応じてChartDataのノーツ配列をフィルタリング
	void ApplyPlayModeFilter(kson::ChartData& chartData, const PlayOption& playOption);
}
