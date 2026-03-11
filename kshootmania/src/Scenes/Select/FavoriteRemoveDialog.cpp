#include "FavoriteRemoveDialog.hpp"
#include "Input/Cursor/CursorInput.hpp"

FavoriteRemoveDialog::FavoriteRemoveDialog(const std::shared_ptr<noco::Canvas>& canvas)
	: m_canvas(canvas)
	, m_menu(LinearMenu::CreateInfoWithCursorMinMax{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Horizontal,
			.buttonFlags = CursorButtonFlags::kArrowOrBT,
		},
		.cursorMin = static_cast<int32>(FavoriteRemoveChoice::No),
		.cursorMax = static_cast<int32>(FavoriteRemoveChoice::Yes),
		.cyclic = IsCyclicMenuYN::Yes,
		.defaultCursor = static_cast<int32>(FavoriteRemoveChoice::No),
	})
{
}

void FavoriteRemoveDialog::show()
{
	// デフォルトはNoを選択
	m_menu.setCursor(static_cast<int32>(FavoriteRemoveChoice::No));

	m_isVisible = true;
	m_canvas->setParamValue(U"dialog_favoriteRemoveDialogVisible", true);
}

void FavoriteRemoveDialog::hide()
{
	m_isVisible = false;
	m_canvas->setParamValue(U"dialog_favoriteRemoveDialogVisible", false);
}

bool FavoriteRemoveDialog::isVisible() const
{
	return m_isVisible;
}

void FavoriteRemoveDialog::update()
{
	if (!m_isVisible)
	{
		return;
	}

	m_menu.update();

	// Canvasパラメータに反映
	m_canvas->setParamValue(U"dialog_favoriteRemoveDialogCursorIndex", m_menu.cursor());
}

FavoriteRemoveChoice FavoriteRemoveDialog::selectedChoice() const
{
	return static_cast<FavoriteRemoveChoice>(m_menu.cursor());
}
