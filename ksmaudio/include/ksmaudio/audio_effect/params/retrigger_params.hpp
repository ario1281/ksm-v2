#pragma once
#include "ksmaudio/audio_effect/audio_effect_param.hpp"

namespace ksmaudio::AudioEffect
{
	struct RetriggerDSPParams
	{
		float secUntilTrigger = -1.0f; // Note: Negative value will be just ignored
		bool updateTrigger = false;
		float waveLength = 0.0f;
		float rate = 0.7f;
		float mix = 1.0f;
	};

	struct RetriggerParams
	{
		Param updatePeriod = DefineParam(Type::kLength, "1/2");
		Param waveLength = DefineParam(Type::kLength, "0");
		Param rate = DefineParam(Type::kRate, "70%");
		Param updateTrigger = DefineParam(Type::kSwitch, "off");
		Param mix = DefineParam(Type::kRate, "0%>100%");

		const std::unordered_map<ParamID, Param*> dict = {
			{ ParamID::kUpdatePeriod, &updatePeriod },
			{ ParamID::kWaveLength, &waveLength },
			{ ParamID::kRate, &rate },
			{ ParamID::kUpdateTrigger, &updateTrigger },
			{ ParamID::kMix, &mix },
		};

	private:
		// DSP�p�����[�^���updateTrigger��off��on�ɕς�����u�Ԃ���true�ɂ���̂ŁA�O��̒l�������Ă���
		// TODO: ���ꏊ�͖{���ɂ������K�؂Ȃ̂��v����
		bool m_prevRawUpdateTrigger = false;

	public:
		RetriggerDSPParams render(const Status& status, bool isOn)
		{
			// updateTrigger��"Off>OnMin-OnMax"��Off�̃g���K�^�C�~���O�͎��O�Ɍv�Z�ς݂ŕʓrsecUntilTrigger���ŏ�������邽�ߖ�������
			// (�������A"on>off-on"��"on-off"�̏ꍇ�̓v���C����off��on�ɂȂ肤��̂Ŗ��������A3���ׂ�"on"�̏ꍇ�̂ݖ�������BsecUntilTrigger�̕��Ƒ��d�ɍX�V�����ꍇ�����邱�ƂɂȂ邪���p��債�����͂Ȃ�)
			const bool ignoreUpdateTrigger = updateTrigger.valueSet.off && updateTrigger.valueSet.onMin && updateTrigger.valueSet.onMax;

			// DSP�p�����[�^���updateTrigger��off��on�ɕς�����u�Ԃ���true�ɂ���
			const bool rawUpdateTrigger = GetValueAsBool(updateTrigger, status, isOn) && !ignoreUpdateTrigger;
			const bool updateTrigger = !m_prevRawUpdateTrigger && rawUpdateTrigger;
			m_prevRawUpdateTrigger = rawUpdateTrigger;

			return {
				.updateTrigger = updateTrigger,
				.waveLength = GetValue(waveLength, status, isOn),
				.rate = GetValue(rate, status, isOn),
				.mix = GetValue(mix, status, isOn),
				// secUntilTrigger�͗��p��(BasicAudioEffectWithTrigger::updateStatus())�Őݒ肳���̂ł����ł͎w�肵�Ȃ�
			};
		}
	};
}
