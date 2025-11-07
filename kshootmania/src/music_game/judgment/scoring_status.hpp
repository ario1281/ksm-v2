#pragma once
#include "music_game/game_defines.hpp"
#include "judgment_defines.hpp"
#include "combo_status.hpp"

namespace MusicGame::Judgment
{
	class ScoringStatus
	{
	private:
		const int32 m_scoreValueMax = 0;
		const int32 m_gaugeValueMax = 0;
		const GaugeType m_gaugeType = GaugeType::kNormalGauge;

		int32 m_scoreValue = 0;
		int32 m_gaugeValue;
		int32 m_gaugeValueNormal = 0; // NORMAL基準ゲージ値(HARDのGrade計算用)

		ComboStatus m_comboStatus;

		void addGaugeValue(int32 add);

		void subtractGaugeValue(int32 sub);

		double calcGaugePercentageFromValue(int32 gaugeValue, GaugeType gaugeType) const;

	public:
		ScoringStatus(int32 scoreValueMax, int32 gaugeValueMax, GaugeType gaugeType);

		void onChipOrLaserSlamJudgment(Judgment::JudgmentResult result);

		void onLongOrLaserLineJudgment(Judgment::JudgmentResult result);

		int32 score() const;

		/// @brief ゲージのパーセンテージを返す
		/// @return パーセンテージ(0.0～100.0)
		double gaugePercentage(GaugeType gaugeType) const;

		/// @brief ゲージのパーセンテージを整数で返す(切り捨て)
		/// @return パーセンテージ(0～100)
		int32 gaugePercentageInt(GaugeType gaugeType) const;

		/// @brief Grade計算用のゲージパーセンテージを返す
		/// @return パーセンテージ(0.0～100.0)
		double gaugePercentageForGrade() const;

		int32 combo() const;

		int32 maxCombo() const;

		const ComboStats& comboStats() const;

		bool isNoError() const;

		int32 totalJudgedCombo() const;
	};
}
