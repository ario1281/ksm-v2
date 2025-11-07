#pragma once
#include "game_defines.hpp"
#include "kson/chart_data.hpp"

namespace MusicGame
{
	// TurnTableを譜面データに適用
	void ApplyTurnTable(kson::ChartData& chartData, const TurnTable& turnTable);
}
