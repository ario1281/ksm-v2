#pragma once
#include "GameDefines.hpp"
#include "kson/ChartData.hpp"

namespace MusicGame
{
	// TurnTableを譜面データに適用
	void ApplyTurnTable(kson::ChartData& chartData, const TurnTable& turnTable);
}
