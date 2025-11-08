#include "ScoringStatus.hpp"

namespace MusicGame::Judgment
{
	namespace
	{
		// ゲージ種類に応じた上昇率倍率を取得
		double GetGaugeIncreaseRate(GaugeType gaugeType)
		{
			switch (gaugeType)
			{
			case GaugeType::kEasyGauge:
				return kGaugeIncreaseRateEasy;
			case GaugeType::kNormalGauge:
				return kGaugeIncreaseRateNormal;
			case GaugeType::kHardGauge:
				return kGaugeIncreaseRateHard;
			default:
				throw Error{ U"GetGaugeIncreaseRate(): Invalid gauge type (gaugeType:{})"_fmt(static_cast<std::underlying_type_t<GaugeType>>(gaugeType)) };
			}
		}

		// ゲージ種類に応じた下降率倍率を取得(EASY/NORMAL用)
		double GetGaugeDecreaseRate(GaugeType gaugeType)
		{
			switch (gaugeType)
			{
			case GaugeType::kEasyGauge:
				return kGaugeDecreaseRateEasy;
			case GaugeType::kNormalGauge:
				return kGaugeDecreaseRateNormal;
			default:
				throw Error{ U"GetGaugeDecreaseRate(): Invalid gauge type (gaugeType:{})"_fmt(static_cast<std::underlying_type_t<GaugeType>>(gaugeType)) };
			}
		}
	}

	void ScoringStatus::addGaugeValue(int32 add)
	{
		const double rate = GetGaugeIncreaseRate(m_gaugeType);
		const int32 adjustedAdd = static_cast<int32>(add * rate);
		m_gaugeValue = Min(m_gaugeValue + adjustedAdd, m_gaugeValueMax);

		// Grade計算用にNORMAL基準のゲージ値も更新
		const double normalRate = GetGaugeIncreaseRate(GaugeType::kNormalGauge);
		const int32 normalAdd = static_cast<int32>(add * normalRate);
		m_gaugeValueNormal = Min(m_gaugeValueNormal + normalAdd, m_gaugeValueMax);
	}

	void ScoringStatus::subtractGaugeValue(int32 sub)
	{
		int32 adjustedSub = 0;

		if (m_gaugeType == GaugeType::kHardGauge)
		{
			// HARDゲージは固定値を使用
			// 百分率を切り捨てで整数化してから判定
			const int32 currentPercentageInt = gaugePercentageInt(GaugeType::kHardGauge);
			const double rate = currentPercentageInt <= kGaugePercentageThresholdHardWarning ? kGaugeDecreaseRateHardLow : 1.0;
			adjustedSub = static_cast<int32>(sub * rate);
		}
		else
		{
			// EASY/NORMALはm_gaugeValueMaxに対する割合で計算
			const double rate = GetGaugeDecreaseRate(m_gaugeType);
			adjustedSub = static_cast<int32>(sub * rate);
		}

		m_gaugeValue = Max(m_gaugeValue - adjustedSub, 0);

		// Grade計算用にNORMAL基準のゲージ値も更新
		const double normalRate = GetGaugeDecreaseRate(GaugeType::kNormalGauge);
		const int32 normalSub = static_cast<int32>(sub * normalRate);
		m_gaugeValueNormal = Max(m_gaugeValueNormal - normalSub, 0);
	}

	ScoringStatus::ScoringStatus(int32 scoreValueMax, int32 gaugeValueMax, GaugeType gaugeType)
		: m_scoreValueMax(scoreValueMax)
		, m_gaugeValueMax(gaugeValueMax)
		, m_gaugeType(gaugeType)
		, m_gaugeValue(gaugeType == GaugeType::kHardGauge ? kGaugeValueMaxHard : 0)
	{
	}

	void ScoringStatus::onChipOrLaserSlamJudgment(Judgment::JudgmentResult result)
	{
		m_comboStatus.processJudgmentResult(result);

		switch (result)
		{
		case Judgment::JudgmentResult::kCritical:
			m_scoreValue += Judgment::kScoreValueCritical;
			addGaugeValue(kGaugeValueChip);
			break;

		case Judgment::JudgmentResult::kNearFast:
		case Judgment::JudgmentResult::kNearSlow:
			m_scoreValue += Judgment::kScoreValueNear;
			if (m_gaugeType != GaugeType::kHardGauge) // HARDゲージの場合、NEARではゲージ上昇しない
			{
				addGaugeValue(kGaugeValueChipNear);
			}
			break;

		case Judgment::JudgmentResult::kError:
			if (m_gaugeType == GaugeType::kHardGauge)
			{
				subtractGaugeValue(kGaugeDecreaseValueHardChipError);
			}
			else
			{
				subtractGaugeValue(static_cast<int32>(m_gaugeValueMax * kGaugeDecreasePercentByChipError / 100));
			}
			break;

		default:
			assert(false && "Invalid JudgmentResult in doChipJudgment");
			break;
		}
	}

	void ScoringStatus::onLongOrLaserLineJudgment(Judgment::JudgmentResult result)
	{
		m_comboStatus.processJudgmentResult(result);

		switch (result)
		{
		case Judgment::JudgmentResult::kCritical:
			m_scoreValue += Judgment::kScoreValueCritical;
			addGaugeValue(kGaugeValueLong);
			break;

		case Judgment::JudgmentResult::kError:
			if (m_gaugeType == GaugeType::kHardGauge)
			{
				subtractGaugeValue(kGaugeDecreaseValueHardLongError);
			}
			else
			{
				subtractGaugeValue(static_cast<int32>(m_gaugeValueMax * kGaugeDecreasePercentByLongError / 100));
			}
			break;

		default:
			assert(false && "Invalid JudgmentResult in doLongJudgment");
			break;
		}
	}

	int32 ScoringStatus::score() const
	{
		if (m_scoreValueMax == 0)
		{
			return 0;
		}
		return static_cast<int32>(static_cast<int64>(kScoreMax) * m_scoreValue / m_scoreValueMax);
	}

	double ScoringStatus::calcGaugePercentageFromValue(int32 gaugeValue, GaugeType gaugeType) const
	{
		if (gaugeType == GaugeType::kHardGauge)
		{
			return Max(0.0, 100.0 * gaugeValue / kGaugeValueMaxHard);
		}
		else
		{
			if (m_gaugeValueMax == 0)
			{
				return 0.0;
			}
			return 100.0 * gaugeValue / m_gaugeValueMax;
		}
	}

	double ScoringStatus::gaugePercentage(GaugeType gaugeType) const
	{
		return calcGaugePercentageFromValue(m_gaugeValue, gaugeType);
	}

	int32 ScoringStatus::gaugePercentageInt(GaugeType gaugeType) const
	{
		if (gaugeType == GaugeType::kHardGauge)
		{
			return m_gaugeValue / 1000;
		}
		else
		{
			if (m_gaugeValueMax == 0)
			{
				return 0;
			}
			return 100 * m_gaugeValue / m_gaugeValueMax;
		}
	}

	double ScoringStatus::gaugePercentageForGrade() const
	{
		// HARDゲージの場合はNORMAL基準のゲージ値を使用
		if (m_gaugeType == GaugeType::kHardGauge)
		{
			return calcGaugePercentageFromValue(m_gaugeValueNormal, GaugeType::kNormalGauge);
		}
		else
		{
			return calcGaugePercentageFromValue(m_gaugeValue, m_gaugeType);
		}
	}

	int32 ScoringStatus::combo() const
	{
		return m_comboStatus.combo();
	}

	int32 ScoringStatus::maxCombo() const
	{
		return m_comboStatus.maxCombo();
	}

	const ComboStats& ScoringStatus::comboStats() const
	{
		return m_comboStatus.stats();
	}

	bool ScoringStatus::isNoError() const
	{
		return m_comboStatus.isNoError();
	}

	int32 ScoringStatus::totalJudgedCombo() const
	{
		return m_comboStatus.totalJudgedCombo();
	}
}
