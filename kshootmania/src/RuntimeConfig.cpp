#include "RuntimeConfig.hpp"
#include "Ini/ConfigIni.hpp"

namespace
{
	struct Config
	{
		GaugeType gaugeType = GaugeType::kNormalGauge;
		TurnMode turnMode = TurnMode::kNormal;
		JudgmentPlayMode judgmentModeBT = JudgmentPlayMode::kOn;
		JudgmentPlayMode judgmentModeFX = JudgmentPlayMode::kOn;
		JudgmentPlayMode judgmentModeLaser = JudgmentPlayMode::kOn;
		double playbackSpeed = 1.0;
	};

	Config g_config;
}

namespace RuntimeConfig
{
	GaugeType GetGaugeType()
	{
		return g_config.gaugeType;
	}

	void SetGaugeType(GaugeType gaugeType)
	{
		g_config.gaugeType = gaugeType;
	}

	TurnMode GetTurnMode()
	{
		return g_config.turnMode;
	}

	void SetTurnMode(TurnMode turnMode)
	{
		g_config.turnMode = turnMode;
	}

	JudgmentPlayMode GetJudgmentPlayModeBT()
	{
		return g_config.judgmentModeBT;
	}

	void SetJudgmentPlayModeBT(JudgmentPlayMode mode)
	{
		g_config.judgmentModeBT = mode;
	}

	JudgmentPlayMode GetJudgmentPlayModeFX()
	{
		return g_config.judgmentModeFX;
	}

	void SetJudgmentPlayModeFX(JudgmentPlayMode mode)
	{
		g_config.judgmentModeFX = mode;
	}

	JudgmentPlayMode GetJudgmentPlayModeLaser()
	{
		return g_config.judgmentModeLaser;
	}

	void SetJudgmentPlayModeLaser(JudgmentPlayMode mode)
	{
		g_config.judgmentModeLaser = mode;
	}

	double GetPlaybackSpeed()
	{
		return g_config.playbackSpeed;
	}

	void SetPlaybackSpeed(double speed)
	{
		g_config.playbackSpeed = speed;
	}

	void RestoreJudgmentModesFromConfigIni()
	{
		g_config.judgmentModeBT = static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeBT, ConfigIni::Value::JudgmentMode::kOn));
		g_config.judgmentModeFX = static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeFX, ConfigIni::Value::JudgmentMode::kOn));
		g_config.judgmentModeLaser = static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeLaser, ConfigIni::Value::JudgmentMode::kOn));
	}
}
