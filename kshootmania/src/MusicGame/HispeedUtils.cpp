#include "HispeedUtils.hpp"

namespace MusicGame::HispeedUtils
{
	namespace
	{
		constexpr int32 kDefaultXModValue = 10; // x1.0
		constexpr int32 kDefaultOModValue = 400;
		constexpr int32 kDefaultCModValue = 400;
	}

	String ToConfigStringValue(const HispeedSetting& hispeedSetting)
	{
		switch (hispeedSetting.type)
		{
		case HispeedType::XMod:
			return U"x{:02d}"_fmt(hispeedSetting.value);
		case HispeedType::OMod:
			return U"{}"_fmt(hispeedSetting.value);
		case HispeedType::CMod:
			return U"C{}"_fmt(hispeedSetting.value);
		default:
			return U"";
		}
	}

	HispeedSetting FromConfigStringValue(StringView sv)
	{
		if (sv.isEmpty())
		{
			return HispeedSetting{
				.type = HispeedType::OMod,
				.value = kDefaultOModValue,
			};
		}

		switch (sv[0])
		{
		case U'x':
		{
			const int32 value = ParseOr<int32>(sv.substr(sv.starts_with(U"x0") ? 2U : 1U), kDefaultXModValue);
			return HispeedSetting{
				.type = HispeedType::XMod,
				.value = value,
			};
		}
		case U'C':
		{
			const int32 value = ParseOr<int32>(sv.substr(1U), kDefaultCModValue);
			return HispeedSetting{
				.type = HispeedType::CMod,
				.value = value,
			};
		}
		default:
		{
			// 数字のみの場合はo-mod
			const int32 value = ParseOr<int32>(sv, kDefaultOModValue);
			return HispeedSetting{
				.type = HispeedType::OMod,
				.value = value,
			};
		}
		}
	}

	String ToDisplayString(const HispeedSetting& hispeedSetting)
	{
		switch (hispeedSetting.type)
		{
		case HispeedType::XMod:
			return U"x{:.1f}"_fmt(hispeedSetting.value / 10.0);
		case HispeedType::OMod:
			return Format(hispeedSetting.value);
		case HispeedType::CMod:
			return U"C{}"_fmt(hispeedSetting.value);
		default:
			return U"";
		}
	}
}
