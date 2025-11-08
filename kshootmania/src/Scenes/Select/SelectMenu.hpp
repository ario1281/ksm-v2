#pragma once
#include "SelectFolderState.hpp"
#include "UI/ArrayWithLinearMenu.hpp"
#include "SelectDifficultyMenu.hpp"
#include "SelectSongPreview.hpp"
#include "ksmaudio/ksmaudio.hpp"

struct HighScoreInfo;

using PlaySeYN = YesNo<struct PlaySeYN_tag>;
using RefreshSongPreviewYN = YesNo<struct RefreshSongPreviewYN_tag>;
using SaveToConfigIniYN = YesNo<struct SaveToConfigIniYN_tag>;
using SongPreviewOnlyYN = YesNo<struct SongPreviewOnlyYN_tag>;

enum class SelectMenuShakeDirection
{
	kUnspecified,
	kUp,
	kDown,
};

class ISelectMenuItem;

struct SelectMenuEventContext
{
	// Note: FilePathViewやconst FilePath&ではなくFilePathにしているのは意図的
	//       (メニュー項目の再構築が発生すると呼び出し元のFilePathが無効になるので、事前にコピーしておく必要がある)
	std::function<void(FilePath, MusicGame::IsAutoPlayYN)> fnMoveToPlayScene;
	std::function<void(FilePath)> fnOpenDirectory;
	std::function<void()> fnOpenAllFolder;
	std::function<void()> fnCloseFolder;
	std::function<const Texture&(FilePathView)> fnGetJacketTexture;
	std::function<const Texture&(FilePathView)> fnGetIconTexture;
	std::function<void()> fnMoveToNextSubDirSection;
	std::function<void()> fnMoveToPrevSubDirSection;
};

class SelectMenu
{
private:
	static constexpr int32 kNumDisplayItems = 8;
	static constexpr int32 kNumTopItems = kNumDisplayItems / 2;
	static constexpr int32 kNumBottomItems = kNumDisplayItems - kNumTopItems - 1;

	const SelectMenuEventContext m_eventContext;

	std::shared_ptr<noco::Canvas> m_selectSceneCanvas;

	SelectFolderState m_folderState;

	ArrayWithLinearMenu<std::unique_ptr<ISelectMenuItem>> m_menu;

	SelectDifficultyMenu m_difficultyMenu;

	SelectSongPreview m_songPreview;

	const ksmaudio::Sample m_songSelectSe{"se/sel_m.wav"};

	const ksmaudio::Sample m_difficultySelectSe{"se/sel_l.wav"};

	const ksmaudio::Sample m_folderSelectSe{"se/sel_dir.wav"};

	HashTable<String, Texture> m_jacketTextureCache;

	HashTable<String, Texture> m_iconTextureCache;

	bool openDirectory(FilePathView directoryPath, PlaySeYN playSe, RefreshSongPreviewYN refreshSongPreview = RefreshSongPreviewYN::Yes, SaveToConfigIniYN saveToConfigIni = SaveToConfigIniYN::Yes);

	bool openDirectoryWithNameSort(FilePathView directoryPath);

	bool openDirectoryWithLevelSort(FilePathView directoryPath);

	bool openAllFolder(PlaySeYN playSe, RefreshSongPreviewYN refreshSongPreview = RefreshSongPreviewYN::Yes, SaveToConfigIniYN saveToConfigIni = SaveToConfigIniYN::Yes);

	bool openAllFolderWithNameSort();

	bool openAllFolderWithLevelSort();

	void setCursorAndSave(int32 cursor);

	void setCursorToItemByFullPath(FilePathView fullPath);

	void refreshContentCanvasParams();

	void refreshSongPreview();

	void playShakeUpTween();

	void playShakeDownTween();

	void moveToNextSubDirSection();

	void moveToPrevSubDirSection();

	Array<FilePath> getSortedTopLevelFolderDirectories() const;

	void addOtherFolderItemsRotated(FilePathView currentFolderPath);

	void addOtherFolderItemsSimple();

public:
	explicit SelectMenu(const std::shared_ptr<noco::Canvas>& selectSceneCanvas, std::function<void(FilePathView, MusicGame::IsAutoPlayYN)> fnMoveToPlayScene);

	~SelectMenu(); // ヘッダではISelectMenuItemが不完全型なのでソースファイル側で定義

	void update(SongPreviewOnlyYN songPreviewOnly = SongPreviewOnlyYN::No);

	void decide();

	void decideAutoPlay();

	bool isFolderOpen() const;

	void closeFolder(PlaySeYN playSe);

	const ISelectMenuItem& cursorMenuItem() const;

	bool empty() const;

	void fadeOutSongPreviewForExit(Duration duration);

	const Texture& getJacketTexture(FilePathView filePath);

	const Texture& getIconTexture(FilePathView filePath);

	void reloadCurrentDirectory();

	void jumpToAlphabetItem(char32 letter);

	void jumpToNextAlphabet();

	void jumpToPrevAlphabet();

	void jumpToFirst();

	void jumpToLast();

	double getCurrentChartStdBPM() const;

	const HighScoreInfo& getCurrentHighScoreInfo() const;

	void showCurrentItemInFileManager();
};
