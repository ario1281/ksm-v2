#pragma once
#include "kson/ChartData.hpp"
#include "kson/Util/TimingUtils.hpp"
#include "ksmaudio/ksmaudio.hpp"

namespace MusicGame::Audio::AudioEffectUtils
{
	std::set<float> PrecalculateUpdateTriggerTiming(
		const kson::AudioEffectDef& def,
		const kson::Dict<kson::ByPulse<std::string>>& paramChange,
		std::int64_t totalMeasures,
		const kson::ChartData& chartData,
		const kson::TimingCache& timingCache);

	std::set<float> PrecalculateUpdateTriggerTiming(
		const kson::AudioEffectDef& def,
		std::int64_t totalMeasures,
		const kson::ChartData& chartData,
		const kson::TimingCache& timingCache);
}
