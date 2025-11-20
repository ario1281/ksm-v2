#pragma once
#include "ISelectMenuItem.hpp"

class SelectMenuSongItem : public ISelectMenuItem
{
private:
	const FilePath m_fullPath;

	bool m_chartExists = false;

	bool m_isSingleChartItem = false;

	std::array<std::unique_ptr<SelectChartInfo>, kNumDifficulties> m_chartInfos;

	// 単一譜面項目の譜面情報を取得(存在しない場合はnullptrを返す)
	const SelectChartInfo* chartInfoForSingleChartItem() const;

public:
	// fullPathはディレクトリパスの場合は楽曲フォルダ、ファイルパスの場合は単体難易度の譜面として読み込む
	explicit SelectMenuSongItem(FilePathView fullPath);

	virtual ~SelectMenuSongItem() = default;

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual void decideAutoPlay(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	virtual FilePathView fullPath() const override
	{
		return m_fullPath;
	}

	virtual const SelectChartInfo* chartInfoPtr(int difficultyIdx, FallbackForSingleChartYN fallbackForSingleChart = FallbackForSingleChartYN::Yes) const override;

	virtual bool difficultyMenuExists() const override
	{
		return !m_isSingleChartItem;
	}

	bool isSingleChartItem() const
	{
		return m_isSingleChartItem;
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
