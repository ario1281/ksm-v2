#include "play_stats_panel.hpp"
#include "i18n/i18n.hpp"

PlayStatsPanel::PlayStatsPanel(std::shared_ptr<noco::Canvas> canvas)
	: m_canvas(canvas)
{
}

bool PlayStatsPanel::isThreeBTButtonsPressed() const
{
	int32 btPressedCount = 0;
	for (KeyConfig::Button btButton = KeyConfig::kBT_A; btButton <= KeyConfig::kBT_D; ++btButton)
	{
		if (KeyConfig::Pressed(btButton))
		{
			++btPressedCount;
		}
	}
	return btPressedCount >= 3;
}

String PlayStatsPanel::generateStatsText(const HighScoreInfo& highScore, GaugeType gaugeType) const
{
	const int32 playCount = highScore.playCount(gaugeType);
	const int32 clearCount = highScore.clearCount(gaugeType);
	const int32 fullComboCount = highScore.fullComboCount(gaugeType);
	const int32 perfectCount = highScore.perfectCount(gaugeType);

	return U"{}{}\n{}{}\n{}{}\n{}{}"_fmt(
		I18n::Get(I18n::Select::kPlayStatsPlayCount),
		playCount,
		I18n::Get(I18n::Select::kPlayStatsClear),
		clearCount,
		I18n::Get(I18n::Select::kPlayStatsFullCombo),
		fullComboCount,
		I18n::Get(I18n::Select::kPlayStatsPerfect),
		perfectCount
	);
}

void PlayStatsPanel::update(const HighScoreInfo& highScore, GaugeType gaugeType)
{
	const bool shouldBeVisible = isThreeBTButtonsPressed();

	if (shouldBeVisible)
	{
		// 統計情報テキストを生成してCanvasに設定
		const String statsText = generateStatsText(highScore, gaugeType);
		m_canvas->setParamValue(U"overlay_playStatsPanelText", statsText);
	}

	// パネルの表示状態を更新
	m_canvas->setParamValue(U"overlay_playStatsPanelVisible", shouldBeVisible);
	m_isVisible = shouldBeVisible;
}

bool PlayStatsPanel::isVisible() const
{
	return m_isVisible;
}
