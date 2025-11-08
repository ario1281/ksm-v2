#pragma once
#include "kson/ChartData.hpp"
#include "MusicGame/PlayOption.hpp"
#include "ButtonLaneJudgment.hpp"
#include "LaserLaneJudgment.hpp"
#include "JudgmentHandler.hpp"

namespace MusicGame::Judgment
{
	/// @brief ノーツ判定のメイン
	class JudgmentMain
	{
	private:
		const PlayOption m_playOption;
		BTLaneJudgments m_btLaneJudgments;
		FXLaneJudgments m_fxLaneJudgments;
		LaserLaneJudgments m_laserLaneJudgments;

		JudgmentHandler m_judgmentHandler;

	public:
		explicit JudgmentMain(const kson::ChartData& chartData, const kson::TimingCache& timingCache, const PlayOption& playOption);

		void update(const kson::ChartData& chartData, GameStatus& gameStatusRef, ViewStatus& viewStatusRef);

		/// @brief プレイ終了のために判定処理をロックし、残りの未判定ノーツをERROR判定にする
		void lockForExit();

		/// @brief PlayResultを取得
		/// @return PlayResult
		PlayResult playResult() const;

		/// @brief 全てのノーツが判定済みかどうか
		/// @return 全てのノーツが判定済みならtrue
		bool isFinished() const;
	};
}
