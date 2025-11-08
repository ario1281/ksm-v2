#pragma once
#include <CoTaskLib.hpp>
#include "TitleAssets.hpp"
#include "Graphics/TiledTexture.hpp"
#include "ksmaudio/ksmaudio.hpp"

enum TitleMenuItem : int32
{
	kStart = 0,
	kOption,
	kInputGate,
	kExit,

	kItemEnumCount,
};

class TitleMenu
{
private:
	bool m_isAlreadySelected = false;

	LinearMenu m_menu;

	std::shared_ptr<noco::Canvas> m_titleSceneCanvas;

	Stopwatch m_stopwatch { StartImmediately::Yes };

	ksmaudio::Sample m_selectSe{ "se/title_sel.wav" };

	Co::TaskFinishSource<TitleMenuItem> m_selectedMenuItemSource;

	void refreshCanvasMenuCursor();

public:
	explicit TitleMenu(TitleMenuItem defaultMenuitem, const std::shared_ptr<noco::Canvas>& titleSceneCanvas);

	void update();

	Co::Task<TitleMenuItem> selectedMenuItemAsync()
	{
		return m_selectedMenuItemSource.waitForResult();
	}
};
