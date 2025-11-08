#include "SelectMenuDirFolderItem.hpp"
#include "Graphics/FontUtils.hpp"
#include "Common/FsUtils.hpp"

SelectMenuDirFolderItem::SelectMenuDirFolderItem(IsCurrentFolderYN isCurrentFolder, FilePathView fullPath)
	: m_isCurrentFolder(isCurrentFolder)
	, m_fullPath(fullPath)
	, m_displayName(FsUtils::DirectoryNameByDirectoryPath(m_fullPath))
{
}

void SelectMenuDirFolderItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	if (m_isCurrentFolder)
	{
		context.fnCloseFolder();
	}
	else
	{
		context.fnOpenDirectory(m_fullPath);
	}
}

void SelectMenuDirFolderItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isDirectory", true },
		{ U"center_isSubDirectory", false },
		{ U"center_isSong", false },
		{ U"center_title", FolderDisplayNameCenter(m_displayName, m_isCurrentFolder) },
	});
}

void SelectMenuDirFolderItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isDirectory", true },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"title", FolderDisplayNameTopBottom(m_displayName, m_isCurrentFolder) },
	});
}

void SelectMenuDirFolderItem::showInFileManager([[maybe_unused]] int32 difficultyIdx) const
{
	// フォルダをエクスプローラで開く
	System::ShowInFileManager(m_fullPath);
}
