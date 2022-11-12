#pragma once
#include "ksmaudio/audio_effect/audio_effect_param.hpp"

namespace ksmaudio::AudioEffect
{
	struct RetriggerEchoDSPParams
	{
		float secUntilTrigger = -1.0f; // Note: Negative value will be just ignored
		bool updateTrigger = false;
		float waveLength = 0.0f;
		float rate = 0.7f;
		bool fadesOut = false;
		float feedbackLevel = 1.0f;
		float mix = 1.0f;
	};

	struct RetriggerParams
	{
		Param updatePeriod = DefineParam(Type::kLength, "1/2");
		Param waveLength = DefineParam(Type::kLength, "0");
		Param rate = DefineParam(Type::kRate, "70%");
		UpdateTriggerParam updateTrigger = DefineUpdateTriggerParam("off");
		Param mix = DefineParam(Type::kRate, "0%>100%");

		const std::unordered_map<ParamID, Param*> dict = {
			{ ParamID::kUpdatePeriod, &updatePeriod },
			{ ParamID::kWaveLength, &waveLength },
			{ ParamID::kRate, &rate },
			{ ParamID::kUpdateTrigger, &updateTrigger },
			{ ParamID::kMix, &mix },
		};

		RetriggerEchoDSPParams render(const Status& status, std::optional<std::size_t> laneIdx)
		{
			const bool isOn = laneIdx.has_value();
			return {
				.updateTrigger = updateTrigger.render(status, laneIdx),
				.waveLength = GetValue(waveLength, status, isOn),
				.rate = GetValue(rate, status, isOn),
				.fadesOut = false, // Retrigger�ł�false�Œ�
				.feedbackLevel = 1.0f, // Retrigger�ł�1�Œ�
				.mix = GetValue(mix, status, isOn),
				// secUntilTrigger�͗��p��(BasicAudioEffectWithTrigger::updateStatus())�Őݒ肳���̂ł����ł͎w�肵�Ȃ�
			};
		}
	};

	struct EchoParams
	{
		Param updatePeriod = DefineParam(Type::kLength, "0");
		Param waveLength = DefineParam(Type::kLength, "0");
		UpdateTriggerParam updateTrigger = DefineUpdateTriggerParam("off>on");
		Param feedbackLevel = DefineParam(Type::kRate, "100%");
		Param mix = DefineParam(Type::kRate, "0%>100%");

		const std::unordered_map<ParamID, Param*> dict = {
			{ ParamID::kUpdatePeriod, &updatePeriod },
			{ ParamID::kWaveLength, &waveLength },
			{ ParamID::kUpdateTrigger, &updateTrigger },
			{ ParamID::kFeedbackLevel, &feedbackLevel },
			{ ParamID::kMix, &mix },
		};

		RetriggerEchoDSPParams render(const Status& status, std::optional<std::size_t> laneIdx)
		{
			const bool isOn = laneIdx.has_value();
			return {
				.updateTrigger = updateTrigger.render(status, laneIdx),
				.waveLength = GetValue(waveLength, status, isOn),
				.rate = 1.0f, // Echo�ł�1�Œ�
				.fadesOut = true, // Echo�ł�true�Œ�
				.feedbackLevel = GetValue(feedbackLevel, status, isOn),
				.mix = GetValue(mix, status, isOn),
				// secUntilTrigger�͗��p��(BasicAudioEffectWithTrigger::updateStatus())�Őݒ肳���̂ł����ł͎w�肵�Ȃ�
			};
		}
	};
}
