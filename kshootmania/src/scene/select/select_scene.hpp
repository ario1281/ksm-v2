#pragma once
#include <CoTaskLib.hpp>
#include "select_menu.hpp"
#include "select_folder_state.hpp"
#include "input/fx_button_up_detection.hpp"
#include "bt_option_panel.hpp"
#include "play_stats_panel.hpp"

class SelectScene : public Co::UpdaterSceneBase
{
private:
	const KeyConfig::Button m_folderCloseButton;

	ColorF m_fadeOutColor = Palette::Black;

	bool m_skipFadeout = false;

	std::shared_ptr<noco::Canvas> m_canvas;

	SelectMenu m_menu;

	Array<String> m_playerNames;

	FXButtonUpDetection m_fxButtonUpDetection;

	BTOptionPanel m_btOptionPanel;

	PlayStatsPanel m_playStatsPanel;

	void moveToPlayScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay);

	void refreshCanvasPlayerName();

	void updatePlayerSwitching();

	void updateAlphabetJump();

public:
	SelectScene();

	virtual void update() override;

	virtual void draw() const override;

	virtual Co::Task<void> fadeIn() override;

	virtual Co::Task<void> fadeOut() override;
};
