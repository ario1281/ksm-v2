#pragma once
#include <CoTaskLib.hpp>
#include "PlayPrepareAssets.hpp"
#include "MusicGame/UI/HispeedSettingMenu.hpp"
#include "MusicGame/Scroll/HighwayScroll.hpp"

class PlayPrepareScene : public Co::SceneBase
{
private:
	const FilePath m_chartFilePath;

	const MusicGame::IsAutoPlayYN m_isAutoPlay;

	const kson::ChartData m_chartData;

	std::shared_ptr<noco::Canvas> m_canvas;

	MusicGame::HispeedSettingMenu m_hispeedMenu;

	MusicGame::Scroll::HighwayScroll m_highwayScroll;

	Stopwatch m_stopwatchSinceHispeedChange{ StartImmediately::Yes };

public:
	explicit PlayPrepareScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay);

	virtual ~PlayPrepareScene() = default;

	virtual Co::Task<void> start() override;

	void update();

	virtual void draw() const override;

	virtual Co::Task<void> fadeIn() override;

	virtual Co::Task<void> postFadeOut() override;
};
