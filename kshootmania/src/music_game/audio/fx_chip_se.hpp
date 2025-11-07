#pragma once
#include "music_game/game_defines.hpp"
#include "music_game/game_status.hpp"
#include "kson/chart_data.hpp"
#include "kson/util/timing_utils.hpp"
#include "ksmaudio/ksmaudio.hpp"

namespace MusicGame::Audio
{
	class FXChipSE
	{
	private:
		std::unordered_map<std::string, ksmaudio::Sample> m_keySounds;

		std::array<double, kson::kNumFXLanesSZ> m_lastPlayedTimeSecs = { kPastTimeSec, kPastTimeSec };

		bool m_isAutoPlaySE;

		std::array<kson::Pulse, kson::kNumFXLanesSZ> m_autoPlaySELastPulses = { 0, 0 };

		std::map<kson::Pulse, double> m_pulseToSec;

		void updateByNoteTime(const kson::ChartData& chartData, const GameStatus& gameStatus);

		void updateByJudgment(const kson::ChartData& chartData, const GameStatus& gameStatus);

	public:
		explicit FXChipSE(const kson::ChartData& chartData, const kson::TimingCache& timingCache, FilePathView parentPath, bool isAutoPlaySE);

		void update(const kson::ChartData& chartData, const GameStatus& gameStatus);
	};
}
