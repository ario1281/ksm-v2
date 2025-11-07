#pragma once
#include "music_game/scroll/hispeed_setting.hpp"

namespace MusicGame::HispeedUtils
{
	/// @brief ハイスピード設定をConfigIni用文字列に変換
	/// @param hispeedSetting ハイスピード設定
	/// @return 文字列(例: "x10", "400", "C400")
	String ToConfigStringValue(const HispeedSetting& hispeedSetting);

	/// @brief ConfigIni用文字列をハイスピード設定に変換
	/// @param sv 文字列(例: "x10", "400", "C400")
	/// @return HispeedSetting構造体
	HispeedSetting FromConfigStringValue(StringView sv);

	/// @brief ハイスピード設定を表示用文字列に変換
	/// @param hispeedSetting ハイスピード設定
	/// @return 文字列(例: "x2.5", "725", "C725")
	String ToDisplayString(const HispeedSetting& hispeedSetting);
}
