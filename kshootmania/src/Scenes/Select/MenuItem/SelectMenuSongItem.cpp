#include "SelectMenuSongItem.hpp"
#include "Graphics/FontUtils.hpp"
#include "Scenes/Select/SelectDifficultyMenu.hpp"

namespace
{
	std::shared_ptr<noco::Node> GetJacketNode(noco::Canvas& canvas, StringView itemNodeName)
	{
		if (const auto itemNode = canvas.findByName(itemNodeName))
		{
			return itemNode->findByName(U"JacketImage");
		}
		return nullptr;
	}

	std::shared_ptr<noco::Node> GetIconNode(noco::Canvas& canvas, StringView itemNodeName)
	{
		if (const auto itemNode = canvas.findByName(itemNodeName))
		{
			return itemNode->findByName(U"Icon");
		}
		return nullptr;
	}
}

SelectMenuSongItem::SelectMenuSongItem(const FilePath& songDirectoryPath)
	: m_fullPath(songDirectoryPath)
{
	const Array<FilePath> chartFilePaths = FileSystem::DirectoryContents(songDirectoryPath, Recursive::No);
	for (const auto& chartFilePath : chartFilePaths)
	{
		if (FileSystem::Extension(chartFilePath) != kKSHExtension) // Note: FileSystem::Extension()は常に小文字を返すので大文字は考慮不要
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
	if (difficultyIdx < 0 || kNumDifficulties <= difficultyIdx)
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
	context.fnMoveToPlayScene(chartFilePath, MusicGame::IsAutoPlayYN::No);
}

void SelectMenuSongItem::decideAutoPlay(const SelectMenuEventContext& context, int32 difficultyIdx)
{
	if (difficultyIdx < 0 || kNumDifficulties <= difficultyIdx)
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
	context.fnMoveToPlayScene(chartFilePath, MusicGame::IsAutoPlayYN::Yes);
}

const SelectChartInfo* SelectMenuSongItem::chartInfoPtr(int difficultyIdx) const
{
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
	if (difficultyIdx >= 0 && difficultyIdx < kNumDifficulties && m_chartInfos[difficultyIdx] != nullptr)
	{
		const SelectChartInfo* pChartInfo = m_chartInfos[difficultyIdx].get();
		const HighScoreInfo& highScoreInfo = pChartInfo->highScoreInfo();

		// TODO: 一旦NORMALゲージ固定
		const GaugeType gaugeType = GaugeType::kNormalGauge;

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
		const Texture& jacketTexture = context.fnGetJacketTexture(pChartInfo->jacketFilePath());
		if (const auto jacketNode = GetJacketNode(canvas, U"CenterItem"))
		{
			if (const auto sprite = jacketNode->getComponent<noco::Sprite>())
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
		}

		// アイコン画像を設定
		// TODO: タグ等で取得可能にする
		if (const auto iconNode = GetIconNode(canvas, U"CenterItem"))
		{
			if (pChartInfo->iconFilePath().isEmpty())
			{
				// アイコンが指定されていない場合は非表示
				iconNode->setActive(false);
			}
			else
			{
				// アイコンが指定されている場合はテクスチャロードして表示
				const Texture& iconTexture = context.fnGetIconTexture(pChartInfo->iconFilePath());
				iconNode->setActive(!iconTexture.isEmpty());
				if (const auto sprite = iconNode->getComponent<noco::Sprite>())
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
	// 難易度が存在しない場合は代替カーソル値を使用して譜面情報を取得
	// (曲名・アーティスト名・ジャケット画像はカーソル難易度が存在しない場合でも常に描画する必要があるため。
	//  なお、difficultyIdxには既にCenterの項目の代替カーソル値が適用済みである、つまりCenterの項目における存在しない難易度を選択中の場合は存在する難易度の値に置換済みである点に注意。
	//  つまり、Centerの代替カーソル値をもとにさらに代替カーソル値を求めている)
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

	const HighScoreInfo& highScoreInfo = pAltChartInfo->highScoreInfo();
	const GaugeType gaugeType = GaugeType::kNormalGauge; // TODO: ゲージタイプの取得方法を確認

	canvas.setParamValues({
		{ paramNamePrefix + U"isSong", true },
		{ paramNamePrefix + U"isDirectory", false },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"title", pAltChartInfo->title() },
		{ paramNamePrefix + U"artist", pAltChartInfo->artist() },
		{ paramNamePrefix + U"levelIndex", pAltChartInfo->level() },
		{ paramNamePrefix + U"medalIndex", static_cast<int32>(highScoreInfo.medal()) },
		{ paramNamePrefix + U"highScoreGradeIndex", static_cast<int32>(highScoreInfo.grade(gaugeType)) },
	});

	// ジャケット画像を設定
	// TODO: タグ等で取得可能にする
	const Texture jacketTexture = context.fnGetJacketTexture(pAltChartInfo->jacketFilePath());
	if (const auto jacketNode = GetJacketNode(canvas, nodeName))
	{
		if (const auto sprite = jacketNode->getComponent<noco::Sprite>())
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
	}

	// アイコン画像を設定
	// TODO: タグ等で取得可能にする
	if (const auto iconNode = GetIconNode(canvas, nodeName))
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
			if (const auto sprite = iconNode->getComponent<noco::Sprite>())
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
