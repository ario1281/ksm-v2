#include <Siv3D.hpp>
#include <CoTaskLib.hpp>
#include <NocoUI.hpp>
#include "common/frame_rate_limit.hpp"
#include "common/ime_utils.hpp"
#include "common/asset_management.hpp"
#include "addon/auto_mute_addon.hpp"
#include "addon/disable_ime_addon.hpp"
#include "ksmaudio/ksmaudio.hpp"
#include "runtime_config.hpp"
#include "scene/title/title_scene.hpp"
#include "input/key_config.hpp"

#ifdef __APPLE__
#include <ksmplatform_macos/input_method.h>
#endif

void CreateHighScoreBackup()
{
	// 既にバックアップフォルダが存在する場合は何もしない
	if (FileSystem::Exists(U"score_backup"))
	{
		return;
	}

	// scoreフォルダを再帰的にコピー
	FileSystem::Copy(U"score", U"score_backup", CopyOption::UpdateExisting);
}

void KSMMain()
{
	// Escキーによるプログラム終了を無効化
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	// F1キーによるライセンス表示を無効化
	LicenseManager::DisableDefaultTrigger();

	// ウィンドウタイトル
	Window::SetTitle(U"K-Shoot MANIA v2.0.0-alpha2");

	// 実行ファイルのパスをカレントディレクトリに設定
	// (ChangeCurrentDirectoryはここ以外は基本的に使用禁止。どうしても使う必要がある場合は必ずAppDirectoryPathに戻すこと)
	FileSystem::ChangeCurrentDirectory(FsUtils::AppDirectoryPath());

	// デフォルト色を指定
	Scene::SetBackground(Palette::Black);
	Graphics3D::SetGlobalAmbientColor(Palette::White);
	Graphics3D::SetSunColor(Palette::Black);

	// 音声処理のバックエンドを初期化
#ifdef _WIN32
	ksmaudio::Init(s3d::Platform::Windows::Window::GetHWND());
#else
	ksmaudio::Init(nullptr);
#endif

	// config.iniを読み込み
	ConfigIni::Load();

	// ランタイム設定を初期化
	RuntimeConfig::RestoreJudgmentModesFromConfigIni();

	// 言語ファイルを読み込み
	I18n::LoadLanguage(ConfigIni::GetString(ConfigIni::Key::kLanguage));

	// ハイスコアのバックアップを作成
	CreateHighScoreBackup();

	// 画面サイズ反映
	Window::SetToggleFullscreenEnabled(false); // Alt+Enter無効化
	ApplyScreenSizeConfig();

	// アセット一覧を登録
	AssetManagement::RegisterAssets();

	// フレームレート制限
	Graphics::SetVSyncEnabled(false);
	Addon::Register(U"FrameRateLimit", std::make_unique<FrameRateLimit>(300), -100);

	Addon::Register(AutoMuteAddon::kAddonName, std::make_unique<AutoMuteAddon>(), 1);

#if defined(_WIN32) || defined(__APPLE__)
	Addon::Register(DisableIMEAddon::kAddonName, std::make_unique<DisableIMEAddon>(), 2);
#endif

	// 毎フレーム連続してアセット生成した時の警告を無効化
	// (楽曲選択でのスクロールにおいては、正常系でもテクスチャ読み込みが毎フレーム発生するため)
	Profiler::EnableAssetCreationWarning(false);

#if defined(_WIN32) || defined(__APPLE__)
	// 起動時はIME無効化を一度実行
	// (これは起動時のものなので、IME無効化設定とは関係なく必ず実行)
	IMEUtils::DetachIMEContext();
#endif

#ifdef __APPLE__
	// macOS: 英数・かなキーのイベントタップを開始してIME切り替えを防ぐ
	KSMPlatformMacOS_StartBlockingIMEKeys();
#endif

#ifdef _DEBUG
	// ライブラリ側のデバッグ用にコンソール表示(Debugビルドの場合のみ)
	Console.open();
#endif

	// ライブラリ初期化
	Co::Init();
	noco::Init();

	// NocoUIのグローバルデフォルトフォントを設定
	noco::SetGlobalDefaultFont(AssetManagement::SystemFont());

	// メインループ
	const auto sceneRunner = Co::PlaySceneFrom<TitleScene>(TitleMenuItem::kStart).runScoped();
	
	while (System::Update())
	{
#ifdef __APPLE__
		// macOSプラットフォーム特有のキーボード状態を更新
		KeyConfig::UpdatePlatformKeyboard();

		// テキスト編集中かどうかを設定(テキスト編集中はIMEキーをブロックしない)
		KSMPlatformMacOS_SetIsEditingText(noco::IsEditingTextBox());
#endif

		if (sceneRunner.done())
		{
			break;
		}
	}

	// config.iniを保存
	ConfigIni::Save();

#ifdef __APPLE__
	// macOS: 英数・かなキーのイベントタップを停止
	KSMPlatformMacOS_StopBlockingIMEKeys();
#endif

	// 音声のバックエンドを終了
	ksmaudio::Terminate();
}

void Main()
{
	try
	{
		KSMMain();
	}
	catch (const Error& e)
	{
		System::MessageBoxOK(e.what(), MessageBoxStyle::Error);
		throw;
	}
}
