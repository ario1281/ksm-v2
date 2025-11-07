#include "title_menu.hpp"

void TitleMenu::refreshCanvasMenuCursor()
{
	m_titleSceneCanvas->setParamValue(U"menuCursor", ToString(m_menu.cursor()));
}

TitleMenu::TitleMenu(TitleMenuItem defaultMenuitem, const std::shared_ptr<noco::Canvas>& titleSceneCanvas)
	: m_menu(
		LinearMenu::CreateInfoWithEnumCount
		{
			.cursorInputCreateInfo = {
				.type = CursorInput::Type::Vertical,
				.buttonFlags = CursorButtonFlags::kArrowOrBTAllOrLaserAll,
			},
			.enumCount = TitleMenuItem::kItemEnumCount,
			.cyclic = IsCyclicMenuYN::No,
			.defaultCursor = defaultMenuitem,
		})
	, m_titleSceneCanvas(titleSceneCanvas)
{
	refreshCanvasMenuCursor();
}

void TitleMenu::update()
{
	// カーソル変化検出用
	// (LinearMenu::deltaCursor() != 0 だとsetCursorでの直接指定による変化を検出できないため、cursor値の比較を利用している)
	const auto beforeCursor = m_menu.cursor();

	const bool backPressed = !m_isAlreadySelected && KeyConfig::Down(KeyConfig::kBack);
	if (!m_isAlreadySelected && !backPressed)
	{
		m_menu.update();

		if (KeyConfig::Down(KeyConfig::kStart))
		{
			const auto selectedItem = m_menu.cursorAs<TitleMenuItem>();
			m_selectedMenuItemSource.requestFinish(selectedItem);
			m_isAlreadySelected = true;
		}
	}

	// BackボタンでEXITへフォーカス
	if (backPressed)
	{
		m_menu.setCursor(kExit);
	}

	if (m_menu.cursor() != beforeCursor)
	{
		// カーソル位置が変化した場合は効果音を鳴らしてCanvasへ反映
		m_selectSe.play();
		refreshCanvasMenuCursor();
	}
}
