#pragma once
#include "ISelectMenuItem.hpp"

class SelectMenuLevelSectionItem : public ISelectMenuItem
{
private:
	const String m_displayName;

public:
	explicit SelectMenuLevelSectionItem(int32 level);

	virtual ~SelectMenuLevelSectionItem() = default;

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual void decideAutoPlay(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual bool isSubFolderHeading() const override
	{
		return true;
	}

	virtual FilePathView fullPath() const override
	{
		return m_displayName;
	}

	virtual void setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const override;

	virtual void setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView paramNamePrefix, StringView nodeName) const override;
};
