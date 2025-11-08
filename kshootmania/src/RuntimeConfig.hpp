#pragma once
#include "Common/CommonDefines.hpp"

// 実行中のみ保持する設定
// config.iniには保存されず、起動時にデフォルト値にリセットされる
namespace RuntimeConfig
{
	GaugeType GetGaugeType();

	void SetGaugeType(GaugeType gaugeType);

	TurnMode GetTurnMode();

	void SetTurnMode(TurnMode turnMode);

	JudgmentPlayMode GetJudgmentPlayModeBT();

	void SetJudgmentPlayModeBT(JudgmentPlayMode mode);

	JudgmentPlayMode GetJudgmentPlayModeFX();

	void SetJudgmentPlayModeFX(JudgmentPlayMode mode);

	JudgmentPlayMode GetJudgmentPlayModeLaser();

	void SetJudgmentPlayModeLaser(JudgmentPlayMode mode);

	// config.iniから判定モードを復元
	void RestoreJudgmentModesFromConfigIni();
}
