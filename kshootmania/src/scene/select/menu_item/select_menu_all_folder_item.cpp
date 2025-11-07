#include "select_menu_all_folder_item.hpp"

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
	canvas.setParamValues({
		{ U"center_isDirectory", true },
		{ U"center_isSubDirectory", false },
		{ U"center_isSong", false },
		{ U"center_title", FolderDisplayNameCenter(kDisplayName, m_isCurrentFolder) },
	});
}

void SelectMenuAllFolderItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isDirectory", true },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"title", FolderDisplayNameTopBottom(kDisplayName, m_isCurrentFolder) },
	});
}
