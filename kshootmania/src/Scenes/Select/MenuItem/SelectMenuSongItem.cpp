#include "SelectMenuSongItem.hpp"
#include "Graphics/FontUtils.hpp"
#include "Scenes/Select/SelectDifficultyMenu.hpp"
#include "RuntimeConfig.hpp"
#include "NocoExtensions/NocoUtils.hpp"

SelectMenuSongItem::SelectMenuSongItem(FilePathView fullPath)
	: m_fullPath(fullPath)
{
	Array<FilePath> chartFilePaths;

	if (FileSystem::IsFile(fullPath))
	{
		// 個別の譜面ファイルの場合
		if (FileSystem::Extension(fullPath) == kKSHExtension || FileSystem::Extension(fullPath) == kKSONExtension)
		{
			chartFilePaths.emplace_back(fullPath);
			m_isSingleChartItem = true;
		}
	}
	else if (FileSystem::IsDirectory(fullPath))
	{
		// ディレクトリの場合
		chartFilePaths = FileSystem::DirectoryContents(fullPath, Recursive::No);
	}
	else
	{
		Logger << U"[ksm warning] SelectMenuSongItem::SelectMenuSongItem: Path does not exist (path:'{}')"_fmt(fullPath);
	}

	for (const auto& chartFilePath : chartFilePaths)
	{
		if (FileSystem::Extension(chartFilePath) != kKSHExtension && FileSystem::Extension(chartFilePath) != kKSONExtension) // Note: FileSystem::Extension()は常に小文字を返すので大文字は考慮不要
		{
			continue;
		}

		auto chartInfo = std::make_unique<SelectChartInfo>(chartFilePath);

		if (chartInfo->hasError())
		{
			Logger << U"[ksm warning] SelectMenuSongItem::SelectMenuSongItem: KSH Loading Error (error:'{}', chartFilePath:'{}')"_fmt(chartInfo->errorString(), chartFilePath);
			continue;
		}

		int32 difficultyIdx = chartInfo->difficultyIdx();
		if (difficultyIdx < 0 || kNumDifficulties <= difficultyIdx) [[unlikely]]
		{
			// 未知の難易度の場合は一番右の難易度にする
			Logger << U"[ksm warning] SelectMenuSongItem::SelectMenuSongItem: Difficulty index out of range (difficultyIdx:{}, chartFilePath:'{}')"_fmt(difficultyIdx, chartFilePath);
			difficultyIdx = kNumDifficulties - 1;
		}

		if (m_chartInfos[difficultyIdx] != nullptr) [[unlikely]]
		{
			Logger << U"[ksm warning] SelectMenuSongItem::SelectMenuSongItem: Skip duplication (difficultyIdx:{}, chartFilePath:'{}')"_fmt(difficultyIdx, chartFilePath);
			continue;
		}

		m_chartInfos[difficultyIdx] = std::move(chartInfo);

		m_chartExists = true;
	}
}

void SelectMenuSongItem::decide(const SelectMenuEventContext& context, int32 difficultyIdx)
{
	// 単一譜面項目の場合はdifficultyIdxに関係なくchartInfoPtr()がフォールバックするので範囲チェックは行わない
	if (!m_isSingleChartItem && (difficultyIdx < 0 || kNumDifficulties <= difficultyIdx))
	{
		Logger << U"[ksm warning] SelectMenuSongItem::decide: Difficulty index out of range (difficultyIdx:{}, fullPath:'{}')"_fmt(difficultyIdx, m_fullPath);
		return;
	}

	const SelectChartInfo* pChartInfo = chartInfoPtr(difficultyIdx);
	if (pChartInfo == nullptr)
	{
		Logger << U"[ksm warning] SelectMenuSongItem::decide: pChartInfo is null (difficultyIdx:{}, fullPath:'{}')"_fmt(difficultyIdx, m_fullPath);
		return;
	}
	const FilePath chartFilePath = FilePath{ pChartInfo->chartFilePath() };

	// 譜面ファイルの存在チェック
	if (!FileSystem::Exists(chartFilePath))
	{
		System::MessageBoxOK(I18n::Get(I18n::Play::kErrorChartFileNotFound), MessageBoxStyle::Error);
		return;
	}

	context.fnMoveToPlayScene(chartFilePath, MusicGame::IsAutoPlayYN::No, none);
}

void SelectMenuSongItem::decideAutoPlay(const SelectMenuEventContext& context, int32 difficultyIdx)
{
	// 単一譜面項目の場合はdifficultyIdxに関係なくchartInfoPtr()がフォールバックするので範囲チェックは行わない
	if (!m_isSingleChartItem && (difficultyIdx < 0 || kNumDifficulties <= difficultyIdx))
	{
		Logger << U"[ksm warning] SelectMenuSongItem::decideAutoPlay: Difficulty index out of range (difficultyIdx:{}, fullPath:'{}')"_fmt(difficultyIdx, m_fullPath);
		return;
	}

	const SelectChartInfo* pChartInfo = chartInfoPtr(difficultyIdx);
	if (pChartInfo == nullptr)
	{
		Logger << U"[ksm warning] SelectMenuSongItem::decideAutoPlay: pChartInfo is null (difficultyIdx:{}, fullPath:'{}')"_fmt(difficultyIdx, m_fullPath);
		return;
	}
	const FilePath chartFilePath = FilePath{ pChartInfo->chartFilePath() };

	// 譜面ファイルの存在チェック
	if (!FileSystem::Exists(chartFilePath))
	{
		System::MessageBoxOK(I18n::Get(I18n::Play::kErrorChartFileNotFound), MessageBoxStyle::Error);
		return;
	}

	context.fnMoveToPlayScene(chartFilePath, MusicGame::IsAutoPlayYN::Yes, none);
}

const SelectChartInfo* SelectMenuSongItem::chartInfoForSingleChartItem() const
{
	for (int32 i = 0; i < kNumDifficulties; ++i)
	{
		if (m_chartInfos[i] != nullptr)
		{
			return m_chartInfos[i].get();
		}
	}
	return nullptr;
}

const SelectChartInfo* SelectMenuSongItem::chartInfoPtr(int difficultyIdx, FallbackForSingleChartYN fallbackForSingleChart) const
{
	// 単一譜面項目の場合は、fallbackForSingleChartがYesならdifficultyIdxに関係なく保持している譜面情報を返す
	if (m_isSingleChartItem && fallbackForSingleChart)
	{
		return chartInfoForSingleChartItem();
	}

	if (difficultyIdx < 0 || kNumDifficulties <= difficultyIdx)
	{
		assert(false && "difficultyIdx out of range!");
		return nullptr;
	}

	return m_chartInfos[difficultyIdx].get();
}

void SelectMenuSongItem::setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx) const
{
	canvas.setParamValues({
		{ U"center_isSong", true },
		{ U"center_isDirectory", false },
		{ U"center_isSubDirectory", false },
		{ U"center_isCourse", false },
	});

	// 各難易度の存在有無とレベルを設定
	for (int32 i = 0; i < kNumDifficulties; ++i)
	{
		const bool exists = m_chartInfos[i] != nullptr;
		const int32 levelIndex = exists ? m_chartInfos[i]->level() - 1 : -1;
		canvas.setParamValues({
			{ U"center_difficulty{}Enabled"_fmt(i), exists },
			{ U"center_difficulty{}LevelIndex"_fmt(i), levelIndex },
		});
	}

	// 選択中の難易度の情報を設定
	const SelectChartInfo* pChartInfo = nullptr;
	if (m_isSingleChartItem)
	{
		pChartInfo = chartInfoForSingleChartItem();
		if (pChartInfo != nullptr)
		{
			difficultyIdx = pChartInfo->difficultyIdx();
		}
	}
	else
	{
		if (0 <= difficultyIdx && difficultyIdx < kNumDifficulties)
		{
			pChartInfo = m_chartInfos[difficultyIdx].get();
		}
	}

	if (pChartInfo != nullptr)
	{
		const HighScoreInfo& highScoreInfo = pChartInfo->highScoreInfo();

		const GaugeType gaugeType = RuntimeConfig::GetGaugeType();

		canvas.setParamValues({
			{ U"center_title", pChartInfo->title() },
			{ U"center_artist", pChartInfo->artist() },
			{ U"center_bpm", pChartInfo->dispBPM() },
			{ U"center_jacketAuthor", pChartInfo->jacketAuthor() },
			{ U"center_information", pChartInfo->information() },
			{ U"center_chartAuthor", pChartInfo->chartAuthor() },
			{ U"center_difficultyCursorState", U"difficulty{}"_fmt(difficultyIdx) },
			{ U"center_medalIndex", static_cast<int32>(highScoreInfo.medal()) },
			{ U"center_highScoreGradeIndex", static_cast<int32>(highScoreInfo.grade(gaugeType)) },
			{ U"center_highScore", U"{:08d}"_fmt(highScoreInfo.score(gaugeType)) },
			{ U"center_gaugePercentage", ToString(highScoreInfo.percent(gaugeType)) },
		});

		// ジャケット画像を設定
		// TODO: タグ等で取得可能にする
		const Texture jacketTexture = context.fnGetJacketTexture(pChartInfo->jacketFilePath());
		if (const auto sprite = NocoUtils::GetComponentByPath<noco::Sprite>(&canvas, { U"CenterItem", U"Song", U"JacketImage" }))
		{
			sprite->setTexture(jacketTexture);
			if (jacketTexture.isEmpty())
			{
				sprite->setColor(ColorF{ 0.0, 0.0 });
			}
			else
			{
				sprite->setColor(Palette::White);
			}
		}

		// アイコン画像を設定
		// TODO: タグ等で取得可能にする
		if (const auto iconNode = NocoUtils::GetNodeByPath(&canvas, { U"CenterItem", U"Song", U"Icon" }))
		{
			if (pChartInfo->iconFilePath().isEmpty())
			{
				// アイコンが指定されていない場合は非表示
				iconNode->setActive(false);
			}
			else
			{
				// アイコンが指定されている場合はテクスチャロードして表示
				const Texture iconTexture = context.fnGetIconTexture(pChartInfo->iconFilePath());
				iconNode->setActive(!iconTexture.isEmpty());
				if (const auto sprite = NocoUtils::GetComponentByPath<noco::Sprite>(&canvas, { U"CenterItem", U"Song", U"Icon" }))
				{
					sprite->setTexture(iconTexture);
				}
			}
		}

		// TODO: title_img, artist_imgの設定
	}
	else
	{
		throw Error{ U"Invalid difficultyIdx: {} (fullPath={})"_fmt(difficultyIdx, m_fullPath) };
	}
}

void SelectMenuSongItem::setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, int32 difficultyIdx, StringView paramNamePrefix, [[maybe_unused]] StringView nodeName) const
{
	// 中央で選択中の難易度がこの曲に存在するかチェック
	// (difficultyIdxには中央の項目の代替カーソル値が適用済み)
	const SelectChartInfo* pChartInfo = chartInfoPtr(difficultyIdx);

	// 曲名・アーティスト名・ジャケット画像は常に描画するため、代替カーソル値を使用して取得
	const int32 altDifficultyIdx = SelectDifficultyMenu::GetAlternativeCursor(difficultyIdx,
		[this](int32 idx)
		{
			return chartInfoPtr(idx) != nullptr;
		});
	const SelectChartInfo* pAltChartInfo = chartInfoPtr(altDifficultyIdx);
	if (pAltChartInfo == nullptr)
	{
		assert(false && "SelectMenuSongItem::setCanvasParamsTopBottom: pAltChartInfo is null");
		return;
	}

	// レベルとメダルは、中央で選択中の難易度がこの曲に存在する場合のみ表示
	// (単一譜面の場合は常に表示)
	int32 levelIndex = -1; // -1は非表示
	int32 medalIndex = -1; // -1は非表示
	int32 highScoreGradeIndex = 0; // グレードは「-」表示にするため0
	int32 gaugePercentage = 0;
	const SelectChartInfo* pDisplayChartInfo = m_isSingleChartItem ? pAltChartInfo : pChartInfo;
	if (pDisplayChartInfo != nullptr)
	{
		const HighScoreInfo& highScoreInfo = pDisplayChartInfo->highScoreInfo();
		const GaugeType gaugeType = RuntimeConfig::GetGaugeType();
		levelIndex = pDisplayChartInfo->level() - 1;
		medalIndex = static_cast<int32>(highScoreInfo.medal());
		highScoreGradeIndex = static_cast<int32>(highScoreInfo.grade(gaugeType));
		gaugePercentage = highScoreInfo.percent(gaugeType);
	}

	canvas.setParamValues({
		{ paramNamePrefix + U"isSong", true },
		{ paramNamePrefix + U"isDirectory", false },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"isCourse", false },
		{ paramNamePrefix + U"title", pAltChartInfo->title() },
		{ paramNamePrefix + U"artist", pAltChartInfo->artist() },
		{ paramNamePrefix + U"levelIndex", levelIndex },
		{ paramNamePrefix + U"medalIndex", medalIndex },
		{ paramNamePrefix + U"highScoreGradeIndex", highScoreGradeIndex },
		{ paramNamePrefix + U"gaugePercentage", ToString(gaugePercentage) },
	});

	// ジャケット画像を設定
	// TODO: タグ等で取得可能にする
	const Texture jacketTexture = context.fnGetJacketTexture(pAltChartInfo->jacketFilePath());
	if (const auto sprite = NocoUtils::GetComponentByPath<noco::Sprite>(&canvas, { nodeName.data(), U"Song", U"JacketImage" }))
	{
		sprite->setTexture(jacketTexture);
		if (jacketTexture.isEmpty())
		{
			sprite->setColor(ColorF{ 0.0, 0.0 });
		}
		else
		{
			sprite->setColor(Palette::White);
		}
	}

	// アイコン画像を設定
	// TODO: タグ等で取得可能にする
	if (const auto iconNode = NocoUtils::GetNodeByPath(&canvas, { nodeName.data(), U"Song", U"Icon" }))
	{
		if (pAltChartInfo->iconFilePath().isEmpty())
		{
			// アイコンが指定されていない場合は非表示
			iconNode->setActive(false);
		}
		else
		{
			// アイコンが指定されている場合はテクスチャロードして表示
			const Texture iconTexture = context.fnGetIconTexture(pAltChartInfo->iconFilePath());
			iconNode->setActive(!iconTexture.isEmpty());
			if (const auto sprite = NocoUtils::GetComponentByPath<noco::Sprite>(&canvas, { nodeName.data(), U"Song", U"Icon" }))
			{
				sprite->setTexture(iconTexture);
			}
		}
	}

	// TODO: title_img, artist_imgの設定
}

void SelectMenuSongItem::showInFileManager(int32 difficultyIdx) const
{
	// 選択中の難易度の譜面ファイルを取得
	const SelectChartInfo* pChartInfo = chartInfoPtr(difficultyIdx);
	if (pChartInfo == nullptr)
	{
		return;
	}

	// エクスプローラで譜面ファイルを選択状態で開く
	System::ShowInFileManager(pChartInfo->chartFilePath());
}

Optional<HighScoreInfo> SelectMenuSongItem::highScoreInfo(int32 difficultyIdx) const
{
	const SelectChartInfo* chartInfo = chartInfoPtr(difficultyIdx);
	if (chartInfo == nullptr)
	{
		return none;
	}
	return chartInfo->highScoreInfo();
}

Optional<String> SelectMenuSongItem::relativePathToCopy(int32 difficultyIdx) const
{
	const SelectChartInfo* chartInfo = chartInfoPtr(difficultyIdx);
	if (chartInfo == nullptr)
	{
		return none;
	}
	String relativePath = FsUtils::RelativePathFromSongsDir(chartInfo->chartFilePath());
	relativePath.replace(U'\\', U'/');
	return relativePath;
}
