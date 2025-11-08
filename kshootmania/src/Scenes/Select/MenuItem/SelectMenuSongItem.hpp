#pragma once
#include "ISelectMenuItem.hpp"

class SelectMenuSongItem : public ISelectMenuItem
{
private:
	const FilePath m_fullPath;

	bool m_chartExists = false;

	std::array<std::unique_ptr<SelectChartInfo>, kNumDifficulties> m_chartInfos;

public:
	explicit SelectMenuSongItem(const FilePath& songDirectoryPath);

	virtual ~SelectMenuSongItem() = default;

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual void decideAutoPlay(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual FilePathView fullPath() const override
	{
		return m_fullPath;
	}

	virtual const SelectChartInfo* chartInfoPtr(int difficultyIdx) const override;

	virtual bool difficultyMenuExists() const override
	{
		return true;
	}

	bool chartExists() const
	{
		return m_chartExists;
	}

	/// @brief Canvasのパラメータを設定(中央の項目)
	/// @param context イベントコンテキスト
	/// @param canvas 設定対象のCanvas
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	virtual void setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const override;

	/// @brief Canvasのパラメータを設定(上下の項目)
	/// @param context イベントコンテキスト
	/// @param canvas 設定対象のCanvas
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	/// @param paramNamePrefix パラメータ名のプレフィックス(例:"top0_")
	/// @param nodeName ノード名
	virtual void setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView paramNamePrefix, StringView nodeName) const override;

	/// @brief この項目をエクスプローラで表示
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	virtual void showInFileManager(int32 difficultyIdx) const override;
};
