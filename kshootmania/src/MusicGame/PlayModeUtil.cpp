#include "PlayModeUtil.hpp"

namespace MusicGame
{
	void ApplyPlayModeFilter(kson::ChartData& chartData, const PlayOption& playOption)
	{
		// Hideモードの場合、ノーツ配列をクリア
		if (playOption.btJudgmentPlayMode == JudgmentPlayMode::kHide)
		{
			for (auto& lane : chartData.note.bt)
			{
				lane.clear();
			}
		}

		if (playOption.fxJudgmentPlayMode == JudgmentPlayMode::kHide)
		{
			for (auto& lane : chartData.note.fx)
			{
				lane.clear();
			}
		}

		if (playOption.laserJudgmentPlayMode == JudgmentPlayMode::kHide)
		{
			for (auto& lane : chartData.note.laser)
			{
				lane.clear();
			}
		}

		// Offモードでの判定除去はButtonLaneJudgment/LaserLaneJudgmentで処理される
	}
}
