#include "play_scene.hpp"
#include "scene/select/select_scene.hpp"
#include "scene/result/result_scene.hpp"
#include "runtime_config.hpp"
#include "music_game/hispeed_utils.hpp"

namespace
{
	constexpr Duration kFadeDuration = 0.6s;

	constexpr Duration kPlayFinishFadeOutDuration = 2.4s;

	Array<MusicGame::HispeedType> LoadAvailableHispeedTypesFromConfigIni()
	{
		Array<MusicGame::HispeedType> availableTypes;
		availableTypes.reserve(static_cast<std::size_t>(MusicGame::HispeedType::EnumCount));

		if (ConfigIni::GetBool(ConfigIni::Key::kHispeedShowXMod, true))
		{
			availableTypes.push_back(MusicGame::HispeedType::XMod);
		}
		if (ConfigIni::GetBool(ConfigIni::Key::kHispeedShowOMod, true))
		{
			availableTypes.push_back(MusicGame::HispeedType::OMod);
		}
		if (ConfigIni::GetBool(ConfigIni::Key::kHispeedShowCMod, false))
		{
			availableTypes.push_back(MusicGame::HispeedType::CMod);
		}

		if (availableTypes.empty())
		{
			availableTypes.push_back(MusicGame::HispeedType::OMod);
		}

		return availableTypes;
	}

	MusicGame::HispeedSetting LoadHispeedSettingFromConfigIni()
	{
		return MusicGame::HispeedUtils::FromConfigStringValue(ConfigIni::GetString(ConfigIni::Key::kHispeed));
	}

	MusicGame::GameCreateInfo MakeGameCreateInfo(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
	{
		return
		{
			.chartFilePath = FilePath{ chartFilePath },
			.playOption = MusicGame::PlayOption
			{
				.isAutoPlay = isAutoPlay,
				.gaugeType = RuntimeConfig::GetGaugeType(),
				.turnMode = RuntimeConfig::GetTurnMode(),
				.btJudgmentPlayMode = RuntimeConfig::GetJudgmentPlayModeBT(),
				.fxJudgmentPlayMode = RuntimeConfig::GetJudgmentPlayModeFX(),
				.laserJudgmentPlayMode = RuntimeConfig::GetJudgmentPlayModeLaser(),
				.globalOffsetMs = ConfigIni::GetInt(ConfigIni::Key::kGlobalOffset),
				.inputDelayMs = ConfigIni::GetInt(ConfigIni::Key::kInputDelay),
				.laserInputDelayMs = ConfigIni::GetInt(ConfigIni::Key::kLaserInputDelay),
				.audioProcDelayMs = ConfigIni::GetInt(ConfigIni::Key::kAudioProcDelay),
				.visualOffsetMs = ConfigIni::GetInt(ConfigIni::Key::kVisualOffset),
				.isAutoPlaySE = ConfigIni::GetBool(ConfigIni::Key::kAutoPlaySE),
				.noteSkin = [&]()
				{
					const StringView noteSkinStr = ConfigIni::GetString(ConfigIni::Key::kNoteSkin, U"default");
					return noteSkinStr == U"note" ? NoteSkinType::kNote : NoteSkinType::kDefault;
				}(),
				.fastSlowMode = static_cast<FastSlowMode>(ConfigIni::GetInt(ConfigIni::Key::kShowFastSlow, static_cast<int32>(FastSlowMode::kHide))),
				.movieEnabled = ConfigIni::GetInt(ConfigIni::Key::kBGMovie, static_cast<int32>(MovieMode::kOn)) == static_cast<int32>(MovieMode::kOn),
				.availableHispeedTypes = LoadAvailableHispeedTypesFromConfigIni(),
				.hispeedSetting = LoadHispeedSettingFromConfigIni(),
			},
			.assistTickMode = static_cast<AssistTickMode>(ConfigIni::GetInt(ConfigIni::Key::kAssistTick, static_cast<int32>(AssistTickMode::kOff))),
		};
	}
}

PlayScene::PlayScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
	: m_gameMain(MakeGameCreateInfo(chartFilePath, isAutoPlay))
	, m_isAutoPlay(isAutoPlay)
	, m_fadeOutDuration(kFadeDuration)
{
	m_gameMain.start();

	// Playシーンではウィンドウのフォーカスが外れていてもミュートしない
	AutoMuteAddon::SetEnabled(false);
}

PlayScene::~PlayScene()
{
	m_gameMain.terminate();
}

void PlayScene::update()
{
	const auto startFadeOut = m_gameMain.update();

	// Backボタンでリザルト画面に遷移(lockForExit中でも有効)
	processBackButtonInput();

	if (startFadeOut)
	{
		// 譜面終了時にリザルト画面に遷移
		m_fadeOutDuration = kPlayFinishFadeOutDuration;

		if (m_isAutoPlay)
		{
			requestNextScene<SelectScene>();
		}
		else
		{
			const ResultSceneArgs args =
			{
				.chartFilePath = FilePath(m_gameMain.chartFilePath()),
				.chartData = m_gameMain.chartData(), // TODO: shared_ptrでコピーを避ける?
				.playResult = m_gameMain.playResult(),
			};
			requestNextScene<ResultScene>(args);
		}
	}
}

void PlayScene::processBackButtonInput()
{
	if (!KeyConfig::Down(KeyConfig::kBack))
	{
		return;
	}

	m_fadeOutDuration = 0s;
	m_backButtonPressedDuringFadeOut = true;

	// Backボタンを押した後は以降の処理でスコア変動しないようロック
	m_gameMain.lockForExit();

	// 次のシーンで多重に反応しないよう、Backボタンの入力をクリア
	KeyConfig::ClearInput(KeyConfig::kBack);

	if (m_isAutoPlay)
	{
		requestNextScene<SelectScene>();
	}
	else
	{
		const ResultSceneArgs args =
		{
			.chartFilePath = FilePath(m_gameMain.chartFilePath()),
			.chartData = m_gameMain.chartData(), // TODO: shared_ptrでコピーを避ける?
			.playResult = m_gameMain.playResult(),
		};
		requestNextScene<ResultScene>(args);
	}
}

void PlayScene::updateFadeOut()
{
	// フェードアウト中もゲームの更新は継続
	m_gameMain.update();

	// Backボタンでフェードアウトをスキップしてリザルト画面に遷移
	processBackButtonInput();
}

void PlayScene::draw() const
{
	m_gameMain.draw();
}

inline Co::Task<void> PlayScene::fadeIn()
{
	co_await Co::ScreenFadeIn(kFadeDuration);
}

Co::Task<void> PlayScene::fadeOut()
{
	// フェードアウト中もBack入力を受け付けるため、updateFadeOutを実行
	const auto updateRunner = Co::UpdaterTask([this] { updateFadeOut(); }).runScoped();

	m_gameMain.startBGMFadeOut(m_fadeOutDuration);

	co_await Co::Any(
		Co::ScreenFadeOut(m_fadeOutDuration),
		Co::WaitUntil([this] { return m_backButtonPressedDuringFadeOut; }));
}
