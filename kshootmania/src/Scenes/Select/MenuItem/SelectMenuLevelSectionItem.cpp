#include "SelectMenuLevelSectionItem.hpp"

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
	canvas.setSubCanvasParamValuesByTag(U"center", {
		{ U"isSong", false },
		{ U"isDirectory", false },
		{ U"isSubDirectory", true },
		{ U"isCourse", false },
		{ U"title", m_displayName },
	});
}

void SelectMenuLevelSectionItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView tag) const
{
	canvas.setSubCanvasParamValuesByTag(tag, {
		{ U"isSong", false },
		{ U"isDirectory", false },
		{ U"isSubDirectory", true },
		{ U"isCourse", false },
		{ U"title", m_displayName },
	});
}
