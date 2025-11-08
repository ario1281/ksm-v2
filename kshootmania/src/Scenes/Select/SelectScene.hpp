#pragma once
#include <CoTaskLib.hpp>
#include "SelectMenu.hpp"
#include "SelectFolderState.hpp"
#include "Input/FXButtonUpDetection.hpp"
#include "BTOptionPanel.hpp"
#include "PlayStatsPanel.hpp"

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
