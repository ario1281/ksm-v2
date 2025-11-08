#include "SelectMenuFavFolderItem.hpp"

SelectMenuFavFolderItem::SelectMenuFavFolderItem(IsCurrentFolderYN isCurrentFolder, FilePathView fullPath)
	: m_isCurrentFolder(isCurrentFolder)
	, m_fullPath(fullPath)
	, m_displayName(FileSystem::FileName(m_fullPath))
{
}

void SelectMenuFavFolderItem::decide([[maybe_unused]] const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	//Print << U"Not Implemented (SelectMenuFavFolderItem::decide)";
}

void SelectMenuFavFolderItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isDirectory", true },
		{ U"center_isSubDirectory", false },
		{ U"center_isSong", false },
		{ U"center_title", FolderDisplayNameCenter(m_displayName, m_isCurrentFolder) },
	});
}

void SelectMenuFavFolderItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isDirectory", true },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"title", FolderDisplayNameTopBottom(m_displayName, m_isCurrentFolder) },
	});
}
