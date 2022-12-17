#pragma once
#include <string>
#include <set>
#include <optional>
#include <unordered_map>

namespace ksmaudio::AudioEffect
{
	enum class Type
	{
		kUnspecified,
		kLength,
		kWaveLength,
		kSample,
		kSwitch,
		kRate,
		kFreq,
		kPitch,
		kInt,
		kFloat,
		kFilename,
	};

	struct ValueSet
	{
		float off = 0.0f;

		float onMin = 0.0f;

		float onMax = 0.0f;
	};

	float StrToValue(Type type, const std::string& str);

	ValueSet StrToValueSet(Type type, const std::string& str, bool* pIsError = nullptr);

	bool ValueAsBool(float value);

	struct Param
	{
		Type type = Type::kUnspecified;

		ValueSet valueSet;
	};

	struct Status
	{
		float v = 0.0f; // Laser value (0-1)

		float bpm = 120.0f;

		float sec = -100.0f; // Non-zero negative value is used here to avoid update_trigger being processed before playback
	};

	float GetValue(const Param& param, const Status& status, bool isOn);

	bool GetValueAsBool(const Param& param, const Status& status, bool isOn);

	Param DefineParam(Type type, const std::string& valueSetStr);

	class TapestopTriggerParam
	{
	private:
		Param m_innerParam;

		bool m_prevTrigger = false;

		std::optional<std::size_t> m_prevLaneIdx = std::nullopt;

		bool m_reset = false;

	public:
		explicit TapestopTriggerParam(const ValueSet& valueSet)
			: m_innerParam{ .type = Type::kSwitch, .valueSet = valueSet }
		{
		}

		bool render(const Status& status, std::optional<std::size_t> laneIdx)
		{
			const bool isOn = laneIdx.has_value();

			// DSP�p�����[�^���trigger���̂͒ʏ�̃p�����[�^�Ɠ���
			const bool trigger = GetValueAsBool(m_innerParam, status, isOn);

			// trigger��false����true�ɕς�����Ƃ��A�܂���
			// �m�[�c���ɕʂ̃��[���̃m�[�c��ǉ��ŉ�����updateTrigger��off��on�ɂȂ����Ƃ��́Areset�̒l��true�ɂ���
			m_reset = trigger && (!m_prevTrigger || laneIdx != m_prevLaneIdx);

			m_prevTrigger = trigger;
			m_prevLaneIdx = laneIdx;

			return trigger;
		}

		bool getResetValue() const
		{
			return m_reset;
		}

		Param* innerParamPtr()
		{
			return &m_innerParam;
		}
	};

	struct UpdateTriggerParam
	{
	private:
		Param m_innerParam;

		bool m_prevRawUpdateTrigger = false;

		std::optional<std::size_t> m_prevLaneIdx = std::nullopt;

	public:
		explicit UpdateTriggerParam(const ValueSet& valueSet)
			: m_innerParam{ .type = Type::kSwitch, .valueSet = valueSet }
		{
		}

		bool render(const Status& status, std::optional<std::size_t> laneIdx)
		{
			const bool isOn = laneIdx.has_value();

			// updateTrigger��"Off>OnMin-OnMax"��Off�̃g���K�^�C�~���O�͎��O�Ɍv�Z�ς݂ŕʓrsecUntilTrigger���ŏ�������邽�ߖ�������
			// (�������A"on>off-on"��"on-off"�̏ꍇ�̓v���C����off��on�ɂȂ肤��̂Ŗ��������A3���ׂ�"on"�̏ꍇ�̂ݖ�������BsecUntilTrigger�̕��Ƒ��d�ɍX�V�����ꍇ�����邱�ƂɂȂ邪���p��債�����͂Ȃ�)
			const bool ignoreUpdateTrigger = m_innerParam.valueSet.off && m_innerParam.valueSet.onMin && m_innerParam.valueSet.onMax;

			// DSP�p�����[�^���updateTrigger��off��on�ɕς�����u�Ԃ���true�ɂ���
			const bool rawUpdateTrigger = GetValueAsBool(m_innerParam, status, isOn) && !ignoreUpdateTrigger;
			const bool updateTriggerValue = rawUpdateTrigger && (!m_prevRawUpdateTrigger || laneIdx != m_prevLaneIdx); // �m�[�c���ɕʂ̃��[���̃m�[�c��ǉ��ŉ�����updateTrigger��off��on�ɂȂ�ꍇ������̂ŁAlaneIdx�̕ω���OR�����ɓ����
			m_prevRawUpdateTrigger = rawUpdateTrigger;
			m_prevLaneIdx = laneIdx;

			return updateTriggerValue;
		}

		Param* innerParamPtr()
		{
			return &m_innerParam;
		}
	};

	TapestopTriggerParam DefineTapestopTriggerParam(const std::string& valueSetStr);

	UpdateTriggerParam DefineUpdateTriggerParam(const std::string& valueSetStr);

	// �p�����[�^��ID
	// (���s���̊Ǘ��ɂ����g�p���Ȃ��̂ŁA�ォ�疖���ȊO�֗񋓎q��ǉ����Ă����Ȃ�)
	enum class ParamID
	{
		kUnknown,
		kAttackTime,
		kBandwidth,
		kDelay,
		kDepth,
		kFeedback,
		kFeedbackLevel,
		kFilename,
		kFreq,
		kFreq1,
		kFreq2,
		kFreqMax,
		kGain,
		kHoldTime,
		kMix,
		kPeriod,
		kPitch,
		kQ,
		kRate,
		kRatio,
		kReduction,
		kReleaseTime,
		kSpeed,
		kStage,
		kStereoWidth,
		kTrigger,
		kUpdateTrigger,
		kV,
		kVol,
		kWaveLength,
		kUpdatePeriod,
	};

	inline const std::unordered_map<std::string, ParamID> kStrToParamID{
		{ "attack_time", ParamID::kAttackTime },
		{ "delay", ParamID::kDelay },
		{ "depth", ParamID::kDepth },
		{ "feedback_level", ParamID::kFeedbackLevel },
		{ "feedback", ParamID::kFeedback },
		{ "filename", ParamID::kFilename },
		{ "freq", ParamID::kFreq },
		{ "freq_1", ParamID::kFreq1 },
		{ "freq_2", ParamID::kFreq2 },
		{ "freq_max", ParamID::kFreqMax },
		{ "gain", ParamID::kGain },
		{ "hold_time", ParamID::kHoldTime },
		{ "mix", ParamID::kMix },
		{ "period", ParamID::kPeriod },
		{ "pitch", ParamID::kPitch },
		{ "q", ParamID::kQ },
		{ "rate", ParamID::kRate },
		{ "ratio", ParamID::kRatio },
		{ "reduction", ParamID::kReduction },
		{ "release_time", ParamID::kReleaseTime },
		{ "speed", ParamID::kSpeed },
		{ "stage", ParamID::kStage },
		{ "stereo_width", ParamID::kStereoWidth },
		{ "trigger", ParamID::kTrigger },
		{ "update_trigger", ParamID::kUpdateTrigger },
		{ "v", ParamID::kV },
		{ "vol", ParamID::kVol },
		{ "wave_length", ParamID::kWaveLength },
		{ "update_period", ParamID::kUpdatePeriod },
	};

	inline const std::unordered_map<ParamID, Type> kParamIDType{
		{ ParamID::kUnknown, Type::kUnspecified },
		{ ParamID::kAttackTime, Type::kLength },
		{ ParamID::kDelay, Type::kSample },
		{ ParamID::kDepth, Type::kSample },
		{ ParamID::kFeedback, Type::kRate },
		{ ParamID::kFeedbackLevel, Type::kRate },
		{ ParamID::kFilename, Type::kFilename },
		{ ParamID::kFreq, Type::kFreq },
		{ ParamID::kFreq1, Type::kFreq },
		{ ParamID::kFreq2, Type::kFreq },
		{ ParamID::kFreqMax, Type::kFreq },
		{ ParamID::kGain, Type::kRate },
		{ ParamID::kHoldTime, Type::kLength },
		{ ParamID::kMix, Type::kRate },
		{ ParamID::kPeriod, Type::kLength },
		{ ParamID::kPitch, Type::kPitch },
		{ ParamID::kQ, Type::kFloat },
		{ ParamID::kRate, Type::kRate },
		{ ParamID::kRatio, Type::kInt },
		{ ParamID::kReduction, Type::kSample },
		{ ParamID::kReleaseTime, Type::kLength },
		{ ParamID::kSpeed, Type::kRate },
		{ ParamID::kStage, Type::kInt },
		{ ParamID::kStereoWidth, Type::kRate },
		{ ParamID::kTrigger, Type::kSwitch },
		{ ParamID::kUpdateTrigger, Type::kSwitch },
		{ ParamID::kV, Type::kRate },
		{ ParamID::kVol, Type::kRate },
		{ ParamID::kWaveLength, Type::kWaveLength },
		{ ParamID::kUpdatePeriod, Type::kLength },
	};

	using ParamValueSetDict = std::unordered_map<ParamID, ValueSet>;
}
