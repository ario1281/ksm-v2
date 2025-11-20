#include "SelectMenuFavFolderItem.hpp"
#include "Common/FsUtils.hpp"

SelectMenuFavFolderItem::SelectMenuFavFolderItem(IsCurrentFolderYN isCurrentFolder, FilePathView specialPath)
	: m_isCurrentFolder(isCurrentFolder)
	, m_fullPath(specialPath)
	, m_displayName(specialPath.substr(1))
{
}

void SelectMenuFavFolderItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	if (m_isCurrentFolder)
	{
		context.fnCloseFolder();
	}
	else
	{
		context.fnOpenFavoriteFolder(m_fullPath);
	}
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

void SelectMenuFavFolderItem::showInFileManager([[maybe_unused]] int32 difficultyIdx) const
{
	// .favファイルがあるディレクトリをエクスプローラで開く
	const FilePath favFilePath = FileSystem::PathAppend(FsUtils::SongsDirectoryPath(), m_displayName + U".fav");
	if (FileSystem::IsFile(favFilePath))
	{
		System::ShowInFileManager(favFilePath);
	}
}
