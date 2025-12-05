#include "OptionScene.hpp"
#include "OptionAssets.hpp"
#include "Common/IMEUtils.hpp"
#include "RuntimeConfig.hpp"
#include "Scenes/Title/TitleScene.hpp"
#include "Input/InputUtils.hpp"

// TODO: TextureIdxまわりどうにかする

namespace
{
	constexpr Duration kFadeDuration = 0.6s;

	constexpr int32 kHeaderX = 12;
	constexpr int32 kHeaderY = 10;
	constexpr int32 kHeaderWidth = 270;
	constexpr int32 kHeaderHeight = 24;
	constexpr int32 kOptionMenuOffsetX = 20;
	constexpr int32 kOptionMenuOffsetY = 40;
	constexpr int32 kGuideX = 20;
	constexpr int32 kGuideY = 456;

	constexpr int32 kMasterVolumeMin = 0;
	constexpr int32 kMasterVolumeMax = 100;
	constexpr int32 kMasterVolumeDefault = 100;
	constexpr int32 kTimingAdjustMin = -10000;
	constexpr int32 kTimingAdjustMax = 10000;
	constexpr int32 kTimingAdjustDefault = 0;
	constexpr int32 kLaserSignalSensitivityMin = -10000;
	constexpr int32 kLaserSignalSensitivityMax = 10000;
	constexpr int32 kLaserSignalSensitivityDefault = 50;

	std::array<OptionMenu, OptionScene::kOptionMenuTypeEnumCount> MakeOptionMenus()
	{
		using StrPair = std::pair<String, String>;
		using IntStrPair = std::pair<int32, String>;
		using DoubleStrPair = std::pair<double, String>;
		using CreateInfo = OptionMenuField::CreateInfo;

		Array<StrPair> availableLanguageStrPairs;
		for (const auto& language : I18n::GetAvailableLanguageList())
		{
			availableLanguageStrPairs.emplace_back(language, language);
		}

		return {
			OptionMenu(OptionTexture::kMenuKeyValueDisplaySound, {
				CreateInfo::Enum(ConfigIni::Key::kFullScreen, Array<StringView>{
					I18n::Get(I18n::Option::kFullscreenOff),
					I18n::Get(I18n::Option::kFullscreenOn),
				}),
				CreateInfo::Enum(ConfigIni::Key::kWindowResolution, Array<StrPair>{
					StrPair{ U"320,240", U"320x240" },
					StrPair{ U"480,320", U"480x320" },
					StrPair{ U"480,360", U"480x360" },
					StrPair{ U"512,384", U"512x384" },
					StrPair{ U"580,435", U"580x435" },
					StrPair{ U"640,480", U"640x480" },
					StrPair{ U"800,600", U"800x600" },
					StrPair{ U"1024,768", U"1024x768" },
					StrPair{ U"1280,720", U"1280x720" },
					StrPair{ U"1280,1024", U"1280x1024" },
					StrPair{ U"1366,768", U"1366x768" },
					StrPair{ U"1920,1080", U"1920x1080" },
					StrPair{ U"1920,1200", U"1920x1200" },
				}),
				CreateInfo::Enum(ConfigIni::Key::kFullScreenResolution, Array<StrPair>{
					StrPair{ U"640,480", U"640x480" },
					StrPair{ U"800,600", U"800x600" },
					StrPair{ U"1024,600", U"1024x600" },
					StrPair{ U"1024,768", U"1024x768" },
					StrPair{ U"1280,720", U"1280x720" },
					StrPair{ U"1280,800", U"1280x800" },
					StrPair{ U"1280,1024", U"1280x1024" },
					StrPair{ U"1366,768", U"1366x768" },
					StrPair{ U"1920,1080", U"1920x1080" },
					StrPair{ U"1920,1200", U"1920x1200" },
				}),
				CreateInfo::Enum(ConfigIni::Key::kLanguage, availableLanguageStrPairs).setOnChangeCallback(
					[]
					{
						I18n::LoadLanguage(ConfigIni::GetString(ConfigIni::Key::kLanguage));
					}),
				/*CreateInfo::Enum(ConfigIni::Key::kTextureSize, Array<IntStrPair>{
					IntStrPair{ ConfigIni::Value::TextureSize::kSmall, I18n::Get(I18n::Option::kTextureSizeSmall) },
					IntStrPair{ ConfigIni::Value::TextureSize::kMedium, I18n::Get(I18n::Option::kTextureSizeMedium) },
					IntStrPair{ ConfigIni::Value::TextureSize::kLarge, I18n::Get(I18n::Option::kTextureSizeLarge) },
				}),*/
				CreateInfo::Enum(ConfigIni::Key::kBGDisplayMode, Array<IntStrPair>{
					IntStrPair{ ConfigIni::Value::BGDisplayMode::kHide, I18n::Get(I18n::Option::kBGDisplayModeHide) },
					IntStrPair{ ConfigIni::Value::BGDisplayMode::kShowNoLayer, I18n::Get(I18n::Option::kBGDisplayModeShowNoLayer) },
					IntStrPair{ ConfigIni::Value::BGDisplayMode::kShowLayer, I18n::Get(I18n::Option::kBGDisplayModeShowLayer) },
				}).setKeyTextureIdx(5),
				CreateInfo::Enum(ConfigIni::Key::kAlwaysShowOtherFolders, Array<StringView>{
					I18n::Get(I18n::Option::kAlwaysShowOtherFoldersOff),
					I18n::Get(I18n::Option::kAlwaysShowOtherFoldersOn),
				}).setKeyTextureIdx(6),
				CreateInfo::Enum(ConfigIni::Key::kHideAllFolder, Array<StringView>{
					I18n::Get(I18n::Option::kHideAllFolderOff),
					I18n::Get(I18n::Option::kHideAllFolderOn),
				}).setKeyTextureIdx(7),
				CreateInfo::Int(ConfigIni::Key::kMasterVolume, kMasterVolumeMin, kMasterVolumeMax, kMasterVolumeDefault, I18n::Get(I18n::Option::kMasterVolumePercent), 5)
					.setKeyTextureIdx(8)
					.setOnChangeCallback(
						[]
						{
							const int32 volume = ConfigIni::GetInt(ConfigIni::Key::kMasterVolume, kMasterVolumeDefault);
							ksmaudio::SetMasterVolume(volume / 100.0);
						}),
				/*CreateInfo::Enum(ConfigIni::Key::kVsync, Array<StringView>{
					I18n::Get(I18n::Option::kVsyncOff),
					I18n::Get(I18n::Option::kVsyncOn),
				}),*/
			}),
			OptionMenu(OptionTexture::kMenuKeyValueInputJudgment, {
				CreateInfo::Enum(ConfigIni::Key::kJudgmentModeBT, Array<StringView>{
					I18n::Get(I18n::Option::kJudgmentOn),
					I18n::Get(I18n::Option::kJudgmentOff),
					I18n::Get(I18n::Option::kJudgmentAuto),
					I18n::Get(I18n::Option::kJudgmentHide),
				}).setOnChangeCallback([]() {
					RuntimeConfig::SetJudgmentPlayModeBT(static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeBT)));
				}),
				CreateInfo::Enum(ConfigIni::Key::kJudgmentModeFX, Array<StringView>{
					I18n::Get(I18n::Option::kJudgmentOn),
					I18n::Get(I18n::Option::kJudgmentOff),
					I18n::Get(I18n::Option::kJudgmentAuto),
					I18n::Get(I18n::Option::kJudgmentHide),
				}).setOnChangeCallback([]() {
					RuntimeConfig::SetJudgmentPlayModeFX(static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeFX)));
				}),
				CreateInfo::Enum(ConfigIni::Key::kJudgmentModeLaser, Array<StringView>{
					I18n::Get(I18n::Option::kJudgmentOn),
					I18n::Get(I18n::Option::kJudgmentOff),
					I18n::Get(I18n::Option::kJudgmentAuto),
					I18n::Get(I18n::Option::kJudgmentHide),
				}).setOnChangeCallback([]() {
					RuntimeConfig::SetJudgmentPlayModeLaser(static_cast<JudgmentPlayMode>(ConfigIni::GetInt(ConfigIni::Key::kJudgmentModeLaser)));
				}),
				CreateInfo::Enum(ConfigIni::Key::kLaserInputType, Array<IntStrPair>{
					IntStrPair{ ConfigIni::Value::LaserInputType::kKeyboard, I18n::Get(I18n::Option::kLaserInputTypeKeyboard) },
					IntStrPair{ ConfigIni::Value::LaserInputType::kSlider, I18n::Get(I18n::Option::kLaserInputTypeSlider) },
#if !defined(__APPLE__) // macOSではマウスデバイスの取得に入力監視の許可が必要で、配布アプリケーション用に正式なコード署名が必要になる(要Apple Developer Program加入)とみられるため実装済みだが一旦除外
					IntStrPair{ ConfigIni::Value::LaserInputType::kMouseXY, I18n::Get(I18n::Option::kLaserInputTypeMouseXY) },
#endif
					IntStrPair{ ConfigIni::Value::LaserInputType::kAnalogStickXY, I18n::Get(I18n::Option::kLaserInputTypeAnalogStickXY) },
				}).setOnChangeCallback([]() {
					InputUtils::InitKsmaxisForCurrentLaserInput();
				}),
				CreateInfo::Enum(ConfigIni::Key::kAssistTick, Array<StringView>{
					I18n::Get(I18n::Option::kAssistTickOff),
					I18n::Get(I18n::Option::kAssistTickOn),
				}).setKeyTextureIdx(4),
				CreateInfo::Enum(ConfigIni::Key::kDisableIME, Array<StringView>{
					I18n::Get(I18n::Option::kDisableIMEOff),
					I18n::Get(I18n::Option::kDisableIMEOnLow),
					I18n::Get(I18n::Option::kDisableIMEOnMid),
					I18n::Get(I18n::Option::kDisableIMEOnHigh),
				}).setKeyTextureIdx(5),
				CreateInfo::Int(ConfigIni::Key::kInputDelay, kTimingAdjustMin, kTimingAdjustMax, kTimingAdjustDefault, I18n::Get(I18n::Option::kTimingAdjustMs))
				.setAdditionalSuffixes(
					I18n::Get(I18n::Option::kTimingAdjustSuffixNoAdjustment),
					I18n::Get(I18n::Option::kTimingAdjustSuffixLater),
					I18n::Get(I18n::Option::kTimingAdjustSuffixEarlier))
				.setKeyTextureIdx(6),
				CreateInfo::Int(ConfigIni::Key::kLaserInputDelay, kTimingAdjustMin, kTimingAdjustMax, kTimingAdjustDefault, I18n::Get(I18n::Option::kTimingAdjustMs))
				.setAdditionalSuffixes(
					I18n::Get(I18n::Option::kTimingAdjustSuffixNoAdjustment),
					I18n::Get(I18n::Option::kLaserTimingAdjustLater),
					I18n::Get(I18n::Option::kLaserTimingAdjustEarlier))
				.setKeyTextureIdx(7),
				CreateInfo::Enum(ConfigIni::Key::kLaserMouseDirectionX, Array<StringView>{
					I18n::Get(I18n::Option::kLaserMouseDirectionLeftThenRight),
					I18n::Get(I18n::Option::kLaserMouseDirectionRightThenRight),
				}).setKeyTextureIdx(8),
				CreateInfo::Enum(ConfigIni::Key::kLaserMouseDirectionY, Array<StringView>{
					I18n::Get(I18n::Option::kLaserMouseDirectionUpThenRight),
					I18n::Get(I18n::Option::kLaserMouseDirectionDownThenRight),
				}).setKeyTextureIdx(9),
				CreateInfo::Int(ConfigIni::Key::kLaserSignalSensitivity, kLaserSignalSensitivityMin, kLaserSignalSensitivityMax, kLaserSignalSensitivityDefault).setKeyTextureIdx(10), // TODO: additional suffix for zero value
				CreateInfo::Enum(ConfigIni::Key::kSwapLaserLR, Array<StringView>{
					I18n::Get(I18n::Option::kDisabled),
					I18n::Get(I18n::Option::kEnabled),
				}).setKeyTextureIdx(11),
				CreateInfo::Enum(ConfigIni::Key::kSelectCloseFolderKey, Array<StringView>{
					I18n::Get(I18n::Option::kSelectCloseFolderKeyBackspace),
					I18n::Get(I18n::Option::kSelectCloseFolderKeyEsc),
				}).setKeyTextureIdx(12),
				CreateInfo::Enum(ConfigIni::Key::kUse3BTsPlusStartAsBack, Array<StringView>{
					I18n::Get(I18n::Option::kDisabled),
					I18n::Get(I18n::Option::kEnabled),
				}).setKeyTextureIdx(13),
			}),
			OptionMenu(OptionTexture::kMenuKeyValueOther, {
				CreateInfo::Enum(ConfigIni::Key::kHispeedShowXMod, Array<StringView>{
					I18n::Get(I18n::Option::kHispeedTypeHide),
					I18n::Get(I18n::Option::kHispeedTypeShow),
				}),
				CreateInfo::Enum(ConfigIni::Key::kHispeedShowOMod, Array<StringView>{
					I18n::Get(I18n::Option::kHispeedTypeHide),
					I18n::Get(I18n::Option::kHispeedTypeShow),
				}),
				CreateInfo::Enum(ConfigIni::Key::kHispeedShowCMod, Array<StringView>{
					I18n::Get(I18n::Option::kHispeedTypeHide),
					I18n::Get(I18n::Option::kHispeedTypeShow),
				}),
				/*CreateInfo::Enum(ConfigIni::Key::kHideMouseCursor, Array<StringView>{
					I18n::Get(I18n::Option::kHideMouseCursorOff),
					I18n::Get(I18n::Option::kHideMouseCursorOn),
				}).setKeyTextureIdx(5),*/
				CreateInfo::Enum(ConfigIni::Key::kUseNumpadAsArrowKeys, Array<StringView>{
					I18n::Get(I18n::Option::kUseNumpadAsArrowKeysOff),
					I18n::Get(I18n::Option::kUseNumpadAsArrowKeysOnKeyboard),
					I18n::Get(I18n::Option::kUseNumpadAsArrowKeysOnController),
				}).setKeyTextureIdx(6),
			}),
			OptionMenu(OptionTexture::kMenuKeyValueOther/*FIXME*/, {
			}),
		};
	}
}

OptionScene::OptionScene()
	: m_bgTexture(TextureAsset(OptionTexture::kBG))
	, m_headerTiledTexture(OptionTexture::kMenuHeader,
		{
			.row = kOptionMenuTypeEnumCount + 1,
			.sourceScale = SourceScale::k2x,
			.sourceSize = { 540, 48 },
		})
	, m_optionMenus(MakeOptionMenus())
{
	m_bgmStream.play();
	AutoMuteAddon::SetEnabled(true);
}

void OptionScene::update()
{
	if (m_currentOptionMenuIdx.has_value())
	{
		if (m_currentOptionMenuIdx == OptionMenuType::kKeyConfig)
		{
			m_keyConfigMenu.update();
		}
		else
		{
			m_optionMenus[*m_currentOptionMenuIdx].update();
		}

		if (KeyConfig::Down(kButtonBack))
		{
			// ボタン編集中の場合は戻らない
			if (m_currentOptionMenuIdx == OptionMenuType::kKeyConfig && m_keyConfigMenu.isButtonEditingState())
			{
				// 何もしない
			}
			else
			{
				m_currentOptionMenuIdx = none;
			}
		}
	}
	else
	{
		m_topMenu.update();

		if (KeyConfig::Down(kButtonStart))
		{
			m_currentOptionMenuIdx = m_topMenu.cursorAs<OptionMenuType>();
		}
		else if (KeyConfig::Down(kButtonBack))
		{
			exitScene();
		}
	}
}

void OptionScene::draw() const
{
	FitToHeight(m_bgTexture).drawAt(Scene::Center());

	StringView footerStr;
	if (m_currentOptionMenuIdx.has_value())
	{
		// Header
		const int32 headerIdx = *m_currentOptionMenuIdx + 1;
		m_headerTiledTexture(headerIdx).draw(Scaled(kHeaderX), Scaled(kHeaderY));

		if (m_currentOptionMenuIdx == OptionMenuType::kKeyConfig)
		{
			// Key config
			m_keyConfigMenu.draw();
			footerStr = I18n::Get(I18n::Option::kGuideKeyConfig);
		}
		else
		{
			// Menu items
			m_optionMenus[*m_currentOptionMenuIdx].draw(LeftMarginVec() + Vec2{ ScaledByWidth(kOptionMenuOffsetX), Scaled(kOptionMenuOffsetY) });
			footerStr = I18n::Get(I18n::Option::kGuideOption);
		}
	}
	else
	{
		// Top menu
		m_topMenu.draw();

		footerStr = I18n::Get(I18n::Option::kGuideTop);
	}

	// Footer text
	m_font(footerStr).draw(Scaled(18), ScaledByWidth(kGuideX), Scaled(kGuideY), Palette::White);
}

Co::Task<void> OptionScene::fadeIn()
{
	co_await Co::ScreenFadeIn(kFadeDuration);
}

Co::Task<void> OptionScene::fadeOut()
{
	m_bgmStream.setFadeOut(kFadeDuration);
	co_await Co::ScreenFadeOut(kFadeDuration);
}

void OptionScene::exitScene()
{
	ConfigIni::Save();

	// 画面サイズ反映
	ApplyScreenSizeConfig();

	// シーン遷移
	requestNextScene<TitleScene>(TitleMenuItem::kOption);
}
