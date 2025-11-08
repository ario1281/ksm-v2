#pragma once
#include <CoTaskLib.hpp>
#include "TitleMenu.hpp"
#include "TitleAssets.hpp"
#include "ksmaudio/ksmaudio.hpp"

class TitleScene : public Co::SceneBase
{
private:
	ksmaudio::Stream m_bgmStream{ "se/title_bgm.ogg", 1.0, false, false, true };
	ksmaudio::Sample m_enterSe{ "se/title_enter.wav" };

	std::shared_ptr<noco::Canvas> m_canvas;

	TitleMenu m_menu;

	TitleMenuItem m_selectedMenuItem = TitleMenuItem::kStart;

public:
	explicit TitleScene(TitleMenuItem defaultMenuitem);

	virtual Co::Task<void> start() override;

	void update();

	virtual void draw() const override;

	virtual Co::Task<void> fadeIn() override;

	virtual Co::Task<void> fadeOut() override;
};
