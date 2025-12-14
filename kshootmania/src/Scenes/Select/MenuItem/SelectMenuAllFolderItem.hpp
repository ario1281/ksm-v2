#pragma once
#include "ISelectMenuItem.hpp"

class SelectMenuAllFolderItem : public ISelectMenuItem
{
private:
	const IsCurrentFolderYN m_isCurrentFolder;

public:
	static constexpr StringView kAllFolderSpecialPath = U"*"; // FullPathがこの値の場合は例外的にAllフォルダを表す

	explicit SelectMenuAllFolderItem(IsCurrentFolderYN isCurrentFolder);

	virtual ~SelectMenuAllFolderItem() = default;

	virtual FilePathView fullPath() const override
	{
		return kAllFolderSpecialPath;
	}

	virtual bool isFolder() const override
	{
		return true;
	}

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) override;

	/// @brief Canvasのパラメータを設定(中央の項目)
	/// @param context イベントコンテキスト
	/// @param canvas 設定対象のCanvas
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	virtual void setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const override;

	/// @brief Canvasのパラメータを設定(上下の項目)
	/// @param context イベントコンテキスト
	/// @param canvas 設定対象のCanvas
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	/// @param tag SubCanvasのタグ(例:"top1", "bottom2")
	virtual void setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView tag) const override;
};
