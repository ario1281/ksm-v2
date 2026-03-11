#pragma once
#include "ISelectMenuItem.hpp"

class SelectMenuCoursesFolderItem : public ISelectMenuItem
{
private:
	const IsCurrentFolderYN m_isCurrentFolder;
	const FilePath m_fullPath;

public:
	static constexpr StringView kCoursesFolderSpecialPath = U":courses"; // FullPathがこの値の場合は例外的にCoursesフォルダを表す

	explicit SelectMenuCoursesFolderItem(IsCurrentFolderYN isCurrentFolder);

	virtual ~SelectMenuCoursesFolderItem() = default;

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual bool isFolder() const override
	{
		return true;
	}

	virtual FilePathView fullPath() const override
	{
		return m_fullPath;
	}

	virtual void setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const override;

	virtual void setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView tag) const override;

	virtual void showInFileManager(int32 difficultyIdx) const override;
};
