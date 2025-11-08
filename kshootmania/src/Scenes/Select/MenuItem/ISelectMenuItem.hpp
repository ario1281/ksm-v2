#pragma once
#include "Scenes/Select/SelectMenu.hpp"
#include "Scenes/Select/SelectChartInfo.hpp"

struct SelectMenuItemGraphicAssets;

using IsCurrentFolderYN = YesNo<struct IsCurrentFolderYN_tag>;

class ISelectMenuItem
{
protected:
	static String FolderDisplayNameCenter(StringView folderName, IsCurrentFolderYN isCurrentFolder)
	{
		if (isCurrentFolder)
		{
			return U"<<   {}   <<"_fmt(folderName);
		}
		else
		{
			return U">>   {}   >>"_fmt(folderName);
		}
	}

	static String FolderDisplayNameTopBottom(StringView folderName, IsCurrentFolderYN isCurrentFolder)
	{
		if (isCurrentFolder)
		{
			return U"<<   {}     "_fmt(folderName);
		}
		else
		{
			return U"     {}   >>"_fmt(folderName);
		}
	}

public:
	ISelectMenuItem() = default;

	virtual ~ISelectMenuItem() = default;

	virtual void decide(const SelectMenuEventContext& context, int32 difficultyIdx) = 0;

	virtual void decideAutoPlay([[maybe_unused]] const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
	{
	}

	virtual FilePathView fullPath() const = 0;

	virtual const SelectChartInfo* chartInfoPtr([[maybe_unused]] int difficultyIdx) const
	{
		return nullptr;
	}

	virtual bool difficultyMenuExists() const
	{
		return false;
	}

	virtual bool isSubFolderHeading() const
	{
		return false;
	}

	virtual bool isFolder() const
	{
		return false;
	}

	/// @brief Canvasのパラメータを設定(中央の項目)
	/// @param context イベントコンテキスト
	/// @param canvas 設定対象のCanvas
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	virtual void setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const = 0;

	/// @brief Canvasのパラメータを設定(上下の項目)
	/// @param context イベントコンテキスト
	/// @param canvas 設定対象のCanvas
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	/// @param paramNamePrefix パラメータ名のプレフィックス(例:"top0_")
	/// @param nodeName ノード名
	virtual void setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView paramNamePrefix, StringView nodeName) const = 0;

	/// @brief この項目をエクスプローラで表示
	/// @param difficultyIdx 現在選択中の難易度のインデックス(0～3)
	virtual void showInFileManager([[maybe_unused]] int32 difficultyIdx) const
	{
	}
};
