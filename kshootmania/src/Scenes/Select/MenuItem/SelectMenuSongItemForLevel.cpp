#include "SelectMenuSongItemForLevel.hpp"
#include "Graphics/FontUtils.hpp"
#include "RuntimeConfig.hpp"

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

SelectMenuSongItemForLevel::SelectMenuSongItemForLevel(const FilePath& chartFilePath, int32 difficultyIdx)
	: m_fullPath(chartFilePath)
	, m_difficultyIdx(difficultyIdx)
{
	auto chartInfo = std::make_unique<SelectChartInfo>(chartFilePath);

	if (chartInfo->hasError())
	{
		Logger << U"[ksm warning] SelectMenuSongItemForLevel::SelectMenuSongItemForLevel: KSH Loading Error (error:'{}', chartFilePath:'{}')"_fmt(chartInfo->errorString(), chartFilePath);
		return;
	}

	m_chartInfo = std::move(chartInfo);
}

void SelectMenuSongItemForLevel::decide(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	if (m_chartInfo == nullptr)
	{
		Logger << U"[ksm warning] SelectMenuSongItemForLevel::decide: chartInfo is null (fullPath:'{}')"_fmt(m_fullPath);
		return;
	}

	context.fnMoveToPlayScene(m_fullPath, MusicGame::IsAutoPlayYN::No);
}

void SelectMenuSongItemForLevel::decideAutoPlay(const SelectMenuEventContext& context, [[maybe_unused]] int32 difficultyIdx)
{
	if (m_chartInfo == nullptr)
	{
		Logger << U"[ksm warning] SelectMenuSongItemForLevel::decideAutoPlay: chartInfo is null (fullPath:'{}')"_fmt(m_fullPath);
		return;
	}

	context.fnMoveToPlayScene(m_fullPath, MusicGame::IsAutoPlayYN::Yes);
}

const SelectChartInfo* SelectMenuSongItemForLevel::chartInfoPtr([[maybe_unused]] int difficultyIdx) const
{
	return m_chartInfo.get();
}

void SelectMenuSongItemForLevel::setCanvasParamsCenter(const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx) const
{
	if (m_chartInfo == nullptr)
	{
		return;
	}

	canvas.setParamValues({
		{ U"center_isSong", true },
		{ U"center_isDirectory", false },
		{ U"center_isSubDirectory", false },
		{ U"center_title", m_chartInfo->title() },
		{ U"center_artist", m_chartInfo->artist() },
		{ U"center_bpm", m_chartInfo->dispBPM() },
		{ U"center_jacketAuthor", m_chartInfo->jacketAuthor() },
		{ U"center_information", m_chartInfo->information() },
		{ U"center_chartAuthor", m_chartInfo->chartAuthor() },
	});

	const bool exists = true;
	const int32 levelIndex = m_chartInfo->level() - 1;
	canvas.setParamValues({
		{ U"center_difficulty{}Enabled"_fmt(m_difficultyIdx), exists },
		{ U"center_difficulty{}LevelIndex"_fmt(m_difficultyIdx), levelIndex },
	});
	for (int32 i = 0; i < kNumDifficulties; ++i)
	{
		if (i != m_difficultyIdx)
		{
			canvas.setParamValues({
				{ U"center_difficulty{}Enabled"_fmt(i), false },
				{ U"center_difficulty{}LevelIndex"_fmt(i), -1 },
			});
		}
	}

	const HighScoreInfo& highScoreInfo = m_chartInfo->highScoreInfo();
	const GaugeType gaugeType = RuntimeConfig::GetGaugeType();

	canvas.setParamValues({
		{ U"center_difficultyCursorState", U"difficulty{}"_fmt(m_difficultyIdx) },
		{ U"center_medalIndex", static_cast<int32>(highScoreInfo.medal()) },
		{ U"center_highScoreGradeIndex", static_cast<int32>(highScoreInfo.grade(gaugeType)) },
		{ U"center_highScore", U"{:08d}"_fmt(highScoreInfo.score(gaugeType)) },
		{ U"center_gaugePercentage", ToString(highScoreInfo.percent(gaugeType)) },
	});

	const Texture& jacketTexture = context.fnGetJacketTexture(m_chartInfo->jacketFilePath());
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

	if (const auto iconNode = GetIconNode(canvas, U"CenterItem"))
	{
		if (m_chartInfo->iconFilePath().isEmpty())
		{
			iconNode->setActive(false);
		}
		else
		{
			const Texture& iconTexture = context.fnGetIconTexture(m_chartInfo->iconFilePath());
			iconNode->setActive(!iconTexture.isEmpty());
			if (const auto sprite = iconNode->getComponent<noco::Sprite>())
			{
				sprite->setTexture(iconTexture);
			}
		}
	}
}

void SelectMenuSongItemForLevel::setCanvasParamsTopBottom(const SelectMenuEventContext& context, noco::Canvas& canvas, [[maybe_unused]] int32 difficultyIdx, StringView paramNamePrefix, StringView nodeName) const
{
	if (m_chartInfo == nullptr)
	{
		return;
	}

	const HighScoreInfo& highScoreInfo = m_chartInfo->highScoreInfo();
	const GaugeType gaugeType = RuntimeConfig::GetGaugeType();

	canvas.setParamValues({
		{ paramNamePrefix + U"isSong", true },
		{ paramNamePrefix + U"isDirectory", false },
		{ paramNamePrefix + U"isSubDirectory", false },
		{ paramNamePrefix + U"title", m_chartInfo->title() },
		{ paramNamePrefix + U"artist", m_chartInfo->artist() },
		{ paramNamePrefix + U"levelIndex", m_chartInfo->level() },
		{ paramNamePrefix + U"medalIndex", static_cast<int32>(highScoreInfo.medal()) },
		{ paramNamePrefix + U"highScoreGradeIndex", static_cast<int32>(highScoreInfo.grade(gaugeType)) },
	});

	const Texture jacketTexture = context.fnGetJacketTexture(m_chartInfo->jacketFilePath());
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

	if (const auto iconNode = GetIconNode(canvas, nodeName))
	{
		if (m_chartInfo->iconFilePath().isEmpty())
		{
			iconNode->setActive(false);
		}
		else
		{
			const Texture iconTexture = context.fnGetIconTexture(m_chartInfo->iconFilePath());
			iconNode->setActive(!iconTexture.isEmpty());
			if (const auto sprite = iconNode->getComponent<noco::Sprite>())
			{
				sprite->setTexture(iconTexture);
			}
		}
	}
}

void SelectMenuSongItemForLevel::showInFileManager([[maybe_unused]] int32 difficultyIdx) const
{
	if (m_chartInfo == nullptr)
	{
		return;
	}

	// エクスプローラで譜面ファイルを選択状態で開く
	System::ShowInFileManager(m_fullPath);
}
