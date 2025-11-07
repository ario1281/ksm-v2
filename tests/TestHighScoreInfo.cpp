#include <catch2/catch.hpp>
#include "high_score/high_score_info.hpp"

TEST_CASE("Medal from achievement for normal gauge", "[HighScoreInfo][Medal]")
{
	HighScoreInfo highScore;

	// Perfect
	highScore.normalGauge.achievement = Achievement::kPerfect;
	REQUIRE(highScore.medal() == Medal::kPerfect);

	// FullCombo
	highScore.normalGauge.achievement = Achievement::kFullCombo;
	REQUIRE(highScore.medal() == Medal::kFullCombo);

	// Clear
	highScore.normalGauge.achievement = Achievement::kCleared;
	REQUIRE(highScore.medal() == Medal::kClear);

	// NoMedal
	highScore.normalGauge.achievement = Achievement::kNone;
	REQUIRE(highScore.medal() == Medal::kNoMedal);
}

TEST_CASE("Medal from achievement for hard gauge", "[HighScoreInfo][Medal]")
{
	HighScoreInfo highScore;

	// Perfect
	highScore.hardGauge.achievement = Achievement::kPerfect;
	REQUIRE(highScore.medal() == Medal::kPerfect);

	// FullCombo
	highScore.hardGauge.achievement = Achievement::kFullCombo;
	REQUIRE(highScore.medal() == Medal::kFullCombo);

	// HardClear
	highScore.hardGauge.achievement = Achievement::kCleared;
	REQUIRE(highScore.medal() == Medal::kHardClear);

	// NoMedal
	highScore.hardGauge.achievement = Achievement::kNone;
	REQUIRE(highScore.medal() == Medal::kNoMedal);
}

TEST_CASE("Medal from achievement for easy gauge", "[HighScoreInfo][Medal]")
{
	HighScoreInfo highScore;

	// EasyPerfect
	highScore.easyGauge.achievement = Achievement::kPerfect;
	REQUIRE(highScore.medal() == Medal::kEasyPerfect);

	// EasyFullCombo
	highScore.easyGauge.achievement = Achievement::kFullCombo;
	REQUIRE(highScore.medal() == Medal::kEasyFullCombo);

	// EasyClear
	highScore.easyGauge.achievement = Achievement::kCleared;
	REQUIRE(highScore.medal() == Medal::kEasyClear);

	// NoMedal
	highScore.easyGauge.achievement = Achievement::kNone;
	REQUIRE(highScore.medal() == Medal::kNoMedal);
}
