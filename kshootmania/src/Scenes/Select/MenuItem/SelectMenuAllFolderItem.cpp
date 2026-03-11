#include "SelectMenuAllFolderItem.hpp"

namespace
{
	constexpr StringView kDisplayName = U"All";
}

SelectMenuAllFolderItem::SelectMenuAllFolderItem(IsCurrentFolderYN isCurrentFolder)
	: m_isCurrentFolder(isCurrentFolder)
{
}

void SelectMenuAllFolderItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	if (m_isCurrentFolder)
	{
		context.fnCloseFolder();
	}
	else
	{
		context.fnOpenAllFolder();
	}
}

void SelectMenuAllFolderItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setSubCanvasParamValuesByTag(U"center", {
		{ U"isSong", false },
		{ U"isDirectory", true },
		{ U"isSubDirectory", false },
		{ U"isCourse", false },
		{ U"title", FolderDisplayNameCenter(kDisplayName, m_isCurrentFolder) },
	});
}

void SelectMenuAllFolderItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView tag) const
{
	canvas.setSubCanvasParamValuesByTag(tag, {
		{ U"isSong", false },
		{ U"isDirectory", true },
		{ U"isSubDirectory", false },
		{ U"isCourse", false },
		{ U"title", FolderDisplayNameTopBottom(kDisplayName, m_isCurrentFolder) },
	});
}
