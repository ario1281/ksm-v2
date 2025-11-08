#include "SelectScene.hpp"
#include "Scenes/PlayPrepare/PlayPrepareScene.hpp"
#include "Scenes/Title/TitleScene.hpp"
#include "Ini/ConfigIni.hpp"
#include "Common/FsUtils.hpp"
#include "RuntimeConfig.hpp"
#include "Input/PlatformKey.hpp"

namespace
{
	constexpr Duration kFadeInDuration = 0.25s;
	constexpr Duration kFadeOutDuration = 0.4s;

	constexpr FilePathView kSelectSceneUIFilePath = U"ui/scene/select.noco";

	std::shared_ptr<noco::Canvas> LoadSelectSceneCanvas()
	{
		const auto canvas = noco::Canvas::LoadFromFile(kSelectSceneUIFilePath);
		if (!canvas)
		{
			throw Error{ U"Failed to load '{}'"_fmt(kSelectSceneUIFilePath) };
		}
		return canvas;
	}

	Array<String> GetPlayerNames()
	{
		Array<String> playerNames;

		for (const auto& dirPath : FileSystem::DirectoryContents(U"score/", Recursive::No))
		{
			if (FileSystem::IsDirectory(dirPath))
			{
				playerNames.push_back(FsUtils::DirectoryNameByDirectoryPath(dirPath));
			}
		}

		if (playerNames.empty())
		{
			playerNames.push_back(U"PLAYER");
		}

		playerNames.sort_by([](const String& a, const String& b)
		{
			return a.lowercased() < b.lowercased();
		});

		return playerNames;
	}
}

void SelectScene::moveToPlayScene(FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlay)
{
	m_fadeOutColor = Palette::White;
	requestNextScene<PlayPrepareScene>(FilePath{ chartFilePath }, isAutoPlay);
}

void SelectScene::refreshCanvasPlayerName()
{
	const String playerName{ ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer) };
	m_canvas->setParamValue(U"playerName", playerName);
}

void SelectScene::updatePlayerSwitching()
{
	const bool btBCPressed = KeyConfig::Pressed(KeyConfig::kBT_B) && KeyConfig::Pressed(KeyConfig::kBT_C);
	if (!btBCPressed)
	{
		return;
	}

	const bool leftDown = KeyConfig::Down(KeyConfig::kLeft);
	const bool rightDown = KeyConfig::Down(KeyConfig::kRight);

	if (!leftDown && !rightDown)
	{
		return;
	}

	const String currentPlayer{ ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer) };

	auto it = std::find(m_playerNames.begin(), m_playerNames.end(), currentPlayer);
	int32 currentIndex = (it != m_playerNames.end()) ? static_cast<int32>(it - m_playerNames.begin()) : 0;

	if (leftDown)
	{
		currentIndex = (currentIndex - 1 + static_cast<int32>(m_playerNames.size())) % static_cast<int32>(m_playerNames.size());
	}
	else if (rightDown)
	{
		currentIndex = (currentIndex + 1) % static_cast<int32>(m_playerNames.size());
	}

	const String newPlayerName = m_playerNames[currentIndex];
	ConfigIni::SetString(ConfigIni::Key::kCurrentPlayer, newPlayerName);
	ConfigIni::Save();

	refreshCanvasPlayerName();
	m_menu.reloadCurrentDirectory();
}

void SelectScene::updateAlphabetJump()
{
	m_fxButtonUpDetection.update();

	// いずれかのBTボタンが押されているかをチェック
	bool btButtonPressed = false;
	for (KeyConfig::Button btButton = KeyConfig::kBT_A; btButton <= KeyConfig::kBT_D; ++btButton)
	{
		if (KeyConfig::Pressed(btButton))
		{
			btButtonPressed = true;
			break;
		}
	}

	// BTボタン押下中 + FX-Lを離した時はリスト先頭へジャンプ
	if (btButtonPressed && m_fxButtonUpDetection.up(KeyConfig::kFX_L))
	{
		m_menu.jumpToFirst();
	}
	// BTボタン押下中 + FX-Rを離した時はリスト末尾へジャンプ
	else if (btButtonPressed && m_fxButtonUpDetection.up(KeyConfig::kFX_R))
	{
		m_menu.jumpToLast();
	}
	// FX-Lを単独で離した時は前のアルファベットグループにジャンプ
	else if (m_fxButtonUpDetection.up(KeyConfig::kFX_L))
	{
		m_menu.jumpToPrevAlphabet();
	}
	// FX-Rを単独で離した時は次のアルファベットグループにジャンプ
	else if (m_fxButtonUpDetection.up(KeyConfig::kFX_R))
	{
		m_menu.jumpToNextAlphabet();
	}
}

SelectScene::SelectScene()
	: m_folderCloseButton(
		ConfigIni::GetInt(ConfigIni::Key::kSelectCloseFolderKey) == ConfigIni::Value::SelectCloseFolderKey::kBackButton
			? static_cast<KeyConfig::Button>(KeyConfig::kBack)
			: static_cast<KeyConfig::Button>(KeyConfig::kBackspace))
	, m_canvas(LoadSelectSceneCanvas())
	, m_menu(m_canvas, [this](FilePathView chartFilePath, MusicGame::IsAutoPlayYN isAutoPlayYN) { moveToPlayScene(chartFilePath, isAutoPlayYN); })
	, m_playerNames(GetPlayerNames())
	, m_fxButtonUpDetection({ KeyShift })
	, m_btOptionPanel(m_canvas)
	, m_playStatsPanel(m_canvas)
{
	AutoMuteAddon::SetEnabled(true);

	// iniから読み込んだプレイヤー名が空文字列の場合は"PLAYER"に修正
	const String currentPlayer{ ConfigIni::GetString(ConfigIni::Key::kCurrentPlayer) };
	if (currentPlayer.isEmpty())
	{
		ConfigIni::SetString(ConfigIni::Key::kCurrentPlayer, U"PLAYER");
	}

	refreshCanvasPlayerName();

	if (m_menu.empty())
	{
		System::MessageBoxOK(U"譜面データが見つかりませんでした。", MessageBoxStyle::Warning);
		m_skipFadeout = true;
		requestNextScene<TitleScene>(TitleMenuItem::kStart);
	}
}

void SelectScene::update()
{
	// BTオプションパネル更新
	const bool needsHighScoreReload = m_btOptionPanel.update(m_menu.getCurrentChartStdBPM());
	if (needsHighScoreReload)
	{
		// ハイスコア情報再読み込み
		m_menu.reloadCurrentDirectory();
	}

	// プレイ統計パネル更新
	m_playStatsPanel.update(m_menu.getCurrentHighScoreInfo(), RuntimeConfig::GetGaugeType());

	// いずれかのパネルが表示中の場合、オーバーレイを表示
	const bool anyPanelVisible = m_btOptionPanel.isVisible() || m_playStatsPanel.isVisible();
	m_canvas->setParamValue(U"overlay_visible", anyPanelVisible);

	// Backキー処理(パネル表示中でも有効にする)
	const bool closeFolder = m_menu.isFolderOpen() && KeyConfig::Down(m_folderCloseButton/* ← kBackspace・kBackのいずれかが入っている */);

	// BackSpaceキーまたはBackボタン(Escキー)でフォルダを閉じる
	if (closeFolder)
	{
		m_menu.closeFolder(PlaySeYN::No);
	}

	// Backボタン(Escキー)を押した場合、(フォルダを閉じる状況でなければ)タイトル画面へ戻る
	if (!closeFolder && KeyConfig::Down(KeyConfig::kBack))
	{
		m_menu.fadeOutSongPreviewForExit(kFadeOutDuration);
		m_fadeOutColor = Palette::Black;
		requestNextScene<TitleScene>(TitleMenuItem::kStart);
		return;
	}

	// Ctrl+O: 選択中の項目をエクスプローラで表示
	if (PlatformKey::KeyCommandControl.pressed() && KeyO.down())
	{
		m_menu.showCurrentItemInFileManager();
	}

	if (anyPanelVisible)
	{
		// パネル表示中は選曲画面の操作を無効化
		// ただし、アルファベット前後ジャンプ(BTを押しながらFX-L/R)とBackキーは有効にする
		updateAlphabetJump();

		// 楽曲プレビューの更新は実行
		m_menu.update(SongPreviewOnlyYN::Yes);

		m_canvas->update();
		return;
	}

	updatePlayerSwitching();

	// 各種操作と干渉しないようCtrl・Shiftキー押下中は無視
	if (!PlatformKey::KeyCommandControl.pressed() && !KeyShift.pressed())
	{
		m_menu.update();
	}

	// スタートボタンを押した場合、フォルダを開く または プレイ開始
	// Shift+スタートボタンの場合はオートプレイ開始
	if (KeyConfig::Down(KeyConfig::kStart))
	{
		if (KeyShift.pressed())
		{
			m_menu.decideAutoPlay();
		}
		else
		{
			m_menu.decide();
		}
	}

	// オートプレイボタン(F11)を押した場合、オートプレイ開始
	if (KeyConfig::Down(KeyConfig::kAutoPlay))
	{
		m_menu.decideAutoPlay();
	}

	// Shift+A〜Zでアルファベットジャンプ
	if (KeyShift.pressed())
	{
		if (KeyA.down()) m_menu.jumpToAlphabetItem(U'A');
		else if (KeyB.down()) m_menu.jumpToAlphabetItem(U'B');
		else if (KeyC.down()) m_menu.jumpToAlphabetItem(U'C');
		else if (KeyD.down()) m_menu.jumpToAlphabetItem(U'D');
		else if (KeyE.down()) m_menu.jumpToAlphabetItem(U'E');
		else if (KeyF.down()) m_menu.jumpToAlphabetItem(U'F');
		else if (KeyG.down()) m_menu.jumpToAlphabetItem(U'G');
		else if (KeyH.down()) m_menu.jumpToAlphabetItem(U'H');
		else if (KeyI.down()) m_menu.jumpToAlphabetItem(U'I');
		else if (KeyJ.down()) m_menu.jumpToAlphabetItem(U'J');
		else if (KeyK.down()) m_menu.jumpToAlphabetItem(U'K');
		else if (KeyL.down()) m_menu.jumpToAlphabetItem(U'L');
		else if (KeyM.down()) m_menu.jumpToAlphabetItem(U'M');
		else if (KeyN.down()) m_menu.jumpToAlphabetItem(U'N');
		else if (KeyO.down()) m_menu.jumpToAlphabetItem(U'O');
		else if (KeyP.down()) m_menu.jumpToAlphabetItem(U'P');
		else if (KeyQ.down()) m_menu.jumpToAlphabetItem(U'Q');
		else if (KeyR.down()) m_menu.jumpToAlphabetItem(U'R');
		else if (KeyS.down()) m_menu.jumpToAlphabetItem(U'S');
		else if (KeyT.down()) m_menu.jumpToAlphabetItem(U'T');
		else if (KeyU.down()) m_menu.jumpToAlphabetItem(U'U');
		else if (KeyV.down()) m_menu.jumpToAlphabetItem(U'V');
		else if (KeyW.down()) m_menu.jumpToAlphabetItem(U'W');
		else if (KeyX.down()) m_menu.jumpToAlphabetItem(U'X');
		else if (KeyY.down()) m_menu.jumpToAlphabetItem(U'Y');
		else if (KeyZ.down()) m_menu.jumpToAlphabetItem(U'Z');
	}

	updateAlphabetJump();

	m_canvas->update();
}

void SelectScene::draw() const
{
	m_canvas->draw();
}

Co::Task<void> SelectScene::fadeIn()
{
	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeIn(kFadeInDuration);
}

Co::Task<void> SelectScene::fadeOut()
{
	if (m_skipFadeout)
	{
		co_return;
	}

	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeOut(kFadeOutDuration, m_fadeOutColor);
}
