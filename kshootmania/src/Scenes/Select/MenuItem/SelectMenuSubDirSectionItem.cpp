#include "SelectMenuSubDirSectionItem.hpp"
#include "Common/FsUtils.hpp"

SelectMenuSubDirSectionItem::SelectMenuSubDirSectionItem(FilePathView fullPath, const Optional<String>& customDisplayName)
	: m_fullPath(fullPath)
	, m_displayName(customDisplayName.value_or(FsUtils::DirectoryNameByDirectoryPath(m_fullPath)))
{
}

void SelectMenuSubDirSectionItem::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	context.fnMoveToNextSubDirSection();
}

void SelectMenuSubDirSectionItem::decideAutoPlay(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	context.fnMoveToPrevSubDirSection();
}

void SelectMenuSubDirSectionItem::setCanvasParamsCenter([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isDirectory", false },
		{ U"center_isSubDirectory", true },
		{ U"center_isSong", false },
		{ U"center_title", m_displayName },
	});
}

void SelectMenuSubDirSectionItem::setCanvasParamsTopBottom([[maybe_unused]] const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	canvas.setParamValues({
		{ paramNamePrefix + U"isDirectory", false },
		{ paramNamePrefix + U"isSubDirectory", true },
		{ paramNamePrefix + U"isSong", false },
		{ paramNamePrefix + U"title", m_displayName },
	});
}

void SelectMenuSubDirSectionItem::showInFileManager([[maybe_unused]] int32 difficultyIdx) const
{
	// サブフォルダをエクスプローラで開く
	System::ShowInFileManager(m_fullPath);
}
