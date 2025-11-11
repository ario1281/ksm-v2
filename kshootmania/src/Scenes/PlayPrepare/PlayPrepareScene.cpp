#include "PlayPrepareScene.hpp"
#include "Scenes/Select/SelectScene.hpp"
#include "Scenes/Play/PlayScene.hpp"
#include "Scenes/Common/ShowLoadingOneFrame.hpp"
#include "MusicGame/HispeedUtils.hpp"

namespace
{
	constexpr Duration kFadeDuration = 1s;

	constexpr Duration kMinDisplayTime = 1.2s;

	constexpr Duration kFadeOutStartTime = 2.7s;

	constexpr Duration kFadeOutDuration = 0.3s;

	constexpr Duration kAutoEndTime = 3.0s;

	FilePath GetPlayPrepareSceneUIFilePath()
	{
		return FsUtils::GetResourcePath(U"ui/scene/play_prepare.noco");
	}

	std::shared_ptr<noco::Canvas> LoadPlayPrepareSceneCanvas()
	{
		const FilePath uiFilePath = GetPlayPrepareSceneUIFilePath();
		const auto canvas = noco::Canvas::LoadFromFile(uiFilePath);
		if (!canvas)
		{
			throw Error{ U"Failed to load '{}'"_fmt(uiFilePath) };
		}
		return canvas;
	}

	double GetInitialBPM(const kson::ChartData& chartData)
	{
		return chartData.beat.bpm.contains(0) ? chartData.beat.bpm.at(0) : kDefaultBPM;
	}

	MusicGame::HispeedSetting LoadHispeedSettingFromConfigIni()
	{
		return MusicGame::HispeedUtils::FromConfigStringValue(ConfigIni::GetString(ConfigIni::Key::kHispeed));
	}

	void SaveHispeedSettingToConfigIni(const MusicGame::HispeedSetting& hispeedSetting)
	{
		ConfigIni::SetString(ConfigIni::Key::kHispeed, MusicGame::HispeedUtils::ToConfigStringValue(hispeedSetting));
	}
}

PlayPrepareScene::PlayPrepareScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
	: m_chartFilePath(chartFilePath)
	, m_isAutoPlay(isAutoPlay)
	, m_chartData(kson::LoadKSHChartData(chartFilePath.narrow()))
	, m_canvas(LoadPlayPrepareSceneCanvas())
	, m_hispeedMenu(ConfigIni::LoadAvailableHispeedTypes(), LoadHispeedSettingFromConfigIni(), kson::GetEffectiveStdBPM(m_chartData), GetInitialBPM(m_chartData))
	, m_highwayScroll(m_chartData)
{
	const double startBPM = GetInitialBPM(m_chartData);

	// ハイスピード設定でHighwayScrollを更新
	m_highwayScroll.update(m_hispeedMenu.hispeedSetting(), startBPM);

	m_canvas->setParamValues({
		{ U"title", Unicode::FromUTF8(m_chartData.meta.title) },
		{ U"artist", Unicode::FromUTF8(m_chartData.meta.artist) },
		{ U"levelNumber", Format(m_chartData.meta.level) },
		{ U"bpmNumber", Format(static_cast<int32>(startBPM)) },
		{ U"difficultyIndex", m_chartData.meta.difficulty.idx },
		{ U"hispeedValue", MusicGame::HispeedUtils::ToDisplayString(m_hispeedMenu.hispeedSetting()) },
		{ U"hispeedValueEffective", Format(m_highwayScroll.currentHispeed()) },
	});

	// ジャケット画像を設定
	const FilePath parentPath = FileSystem::ParentPath(chartFilePath);
	const String jacketFilename = Unicode::FromUTF8(m_chartData.meta.jacketFilename);
	FilePath jacketPath;

	// 拡張子なしの場合はimgs/jacket内の画像を使用
	if (FileSystem::Extension(jacketFilename).isEmpty())
	{
		jacketPath = FileSystem::PathAppend(U"imgs/jacket", jacketFilename + U".jpg");
	}
	else
	{
		jacketPath = FileSystem::PathAppend(parentPath, jacketFilename);
	}

	const Texture jacketTexture{ jacketPath };
	if (const auto jacketNode = m_canvas->findByName(U"Jacket"))
	{
		if (const auto sprite = jacketNode->getComponent<noco::Sprite>())
		{
			sprite->setTexture(jacketTexture);
		}
	}
}

Co::Task<void> PlayPrepareScene::start()
{
	const auto updateRunner = Co::UpdaterTask([this] { update(); }).runScoped();

	CommonSEAddon::Play(CommonSEType::kSelEnter);

	// ハイスピード変更から一定時間経過、またはBackキーで終了
	while (true)
	{
		const Duration elapsed = m_stopwatchSinceHispeedChange.elapsed();
		if (elapsed >= kAutoEndTime)
		{
			// 自動終了
			SaveHispeedSettingToConfigIni(m_hispeedMenu.hispeedSetting());
			requestNextScene<PlayScene>(m_chartFilePath, m_isAutoPlay);
			break;
		}

		if (KeyConfig::Down(KeyConfig::kBack))
		{
			// Backボタンで選曲画面へ戻る
			SaveHispeedSettingToConfigIni(m_hispeedMenu.hispeedSetting());
			requestNextScene<SelectScene>();
			break;
		}

		if (elapsed >= kMinDisplayTime && KeyConfig::Down(KeyConfig::kStart))
		{
			// 一定時間経過後はStartボタンでスキップ可能
			SaveHispeedSettingToConfigIni(m_hispeedMenu.hispeedSetting());
			requestNextScene<PlayScene>(m_chartFilePath, m_isAutoPlay);
			break;
		}

		co_await Co::NextFrame();
	}
}

void PlayPrepareScene::update()
{
	m_canvas->update();

	const double startBPM = m_chartData.beat.bpm.contains(0) ? m_chartData.beat.bpm.at(0) : kDefaultBPM;

	// ハイスピードメニューの更新
	if (m_hispeedMenu.update(startBPM))
	{
		// ハイスピード設定が変更された場合、タイマーをリセット
		m_stopwatchSinceHispeedChange.restart();

		// HighwayScrollを更新
		m_highwayScroll.update(m_hispeedMenu.hispeedSetting(), startBPM);

		// ハイスピード表示を更新
		m_canvas->setParamValues({
			{ U"hispeedValue", MusicGame::HispeedUtils::ToDisplayString(m_hispeedMenu.hispeedSetting()) },
			{ U"hispeedValueEffective", Format(m_highwayScroll.currentHispeed()) },
		});
	}
}

void PlayPrepareScene::draw() const
{
	m_canvas->draw();

	// フェードアウト描画
	// (途中でハイスピード変更したときにフェードアウトを途中で戻す必要があるため、Co::ScreenFadeOutを使わずに毎フレームの処理で描画)
	const double elapsedSec = m_stopwatchSinceHispeedChange.sF();
	if (elapsedSec >= kFadeOutStartTime.count())
	{
		const double fadeOutProgress = Min((elapsedSec - kFadeOutStartTime.count()) / kFadeOutDuration.count(), 1.0);
		Scene::Rect().draw(ColorF{ 0.0, fadeOutProgress });
	}
}

Co::Task<void> PlayPrepareScene::fadeIn()
{
	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeIn(kFadeDuration, Palette::White);
}

Co::Task<void> PlayPrepareScene::postFadeOut()
{
	co_await ShowLoadingOneFrame::Play(HasBgYN::No);
}
