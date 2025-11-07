#pragma once
#include "iselect_menu_item.hpp"

class SelectMenuSongItemForLevel : public ISelectMenuItem
{
private:
	const FilePath m_fullPath;
	const int32 m_difficultyIdx;
	std::unique_ptr<SelectChartInfo> m_chartInfo;

public:
	SelectMenuSongItemForLevel(const FilePath& chartFilePath, int32 difficultyIdx);

	virtual ~SelectMenuSongItemForLevel() = default;

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual void decideAutoPlay(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual FilePathView fullPath() const override
	{
		return m_fullPath;
	}

	virtual const SelectChartInfo* chartInfoPtr(int difficultyIdx) const override;

	virtual bool difficultyMenuExists() const override
	{
		return false;
	}

	bool hasChart() const
	{
		return m_chartInfo != nullptr;
	}

	virtual void setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const override;

	virtual void setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView paramNamePrefix, StringView nodeName) const override;

	/// @brief この項目をエクスプローラで表示
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	virtual void showInFileManager(int32 difficultyIdx) const override;
};
