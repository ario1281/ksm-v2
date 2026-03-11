#pragma once
#include "MusicGame/GameDefines.hpp"

struct KscKey
{
	GaugeType gaugeType = GaugeType::kNormalGauge;

	TurnMode turnMode = TurnMode::kNormal;

	double playbackSpeed = 1.0;

	JudgmentPlayMode btPlayMode = JudgmentPlayMode::kOn;

	JudgmentPlayMode fxPlayMode = JudgmentPlayMode::kOn;

	JudgmentPlayMode laserPlayMode = JudgmentPlayMode::kOn;

	// キー文字列を取得
	[[nodiscard]]
	String toString() const;

	// gaugeTypeを除いたキー文字列を取得
	String toStringWithoutGaugeType() const;

	KscKey withGaugeType(GaugeType newGaugeType) const;
};
