#include "select_menu_level_section_item.hpp"

SelectMenuLevelSectionItem::SelectMenuLevelSectionItem(int32 level)
	: m_displayName(U"Lv{}"_fmt(level))
{
}

void SelectMenuLevelSectionItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	context.fnMoveToNextSubDirSection();
}

void SelectMenuLevelSectionItem::decideAutoPlay(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	context.fnMoveToPrevSubDirSection();
}

void SelectMenuLevelSectionItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isDirectory", false },
		{ U"center_isSubDirectory", true },
		{ U"center_isSong", false },
		{ U"center_title", m_displayName },
	});
}

void SelectMenuLevelSectionItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isDirectory", false },
		{ paramNamePrefix + U"isSubDirectory", true },
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"title", m_displayName },
	});
}
