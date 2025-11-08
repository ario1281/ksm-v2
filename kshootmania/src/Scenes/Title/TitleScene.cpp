#include "TitleScene.hpp"
#include "Scenes/Select/SelectScene.hpp"
#include "Scenes/Option/OptionScene.hpp"
#include "Scenes/Common/ShowLoadingOneFrame.hpp"

namespace
{
	constexpr Duration kFadeDuration = 0.5s;
	constexpr Duration kFadeDurationExit = 0.8s;

	constexpr FilePathView kTitleSceneUIFilePath = U"ui/scene/title.noco";

	std::shared_ptr<noco::Canvas> LoadTitleSceneCanvas()
	{
		const auto canvas = noco::Canvas::LoadFromFile(kTitleSceneUIFilePath);
		if (!canvas)
		{
			throw Error{ U"Failed to load '{}'"_fmt(kTitleSceneUIFilePath) };
		}
		return canvas;
	}
}

TitleScene::TitleScene(TitleMenuItem defaultMenuitem)
	: m_canvas(LoadTitleSceneCanvas())
	, m_menu(defaultMenuitem, m_canvas)
{
}

Co::Task<void> TitleScene::start()
{
	const auto updateRunner = Co::UpdaterTask([this] { update(); }).runScoped();

	m_bgmStream.play();
	AutoMuteAddon::SetEnabled(true);

	// メニューが選択されるまで待機
	m_selectedMenuItem = co_await m_menu.selectedMenuItemAsync();

	// 効果音を鳴らす
	m_bgmStream.setVolume(0.0);
	m_enterSe.play(); // TODO: シーン遷移時に消えるのを修正
}

void TitleScene::update()
{
	m_menu.update();
	m_canvas->update();
}

void TitleScene::draw() const
{
	m_canvas->draw();
}

Co::Task<void> TitleScene::fadeIn()
{
	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	co_await Co::ScreenFadeIn(kFadeDuration);
}

Co::Task<void> TitleScene::fadeOut()
{
	const auto canvasUpdateRunner = Co::UpdaterTask([this] { m_canvas->update(); }).runScoped();

	// 次のシーンへ遷移
	switch (m_selectedMenuItem)
	{
	case TitleMenuItem::kStart:
		co_await Co::ScreenFadeOut(kFadeDuration);
		requestNextScene<SelectScene>();

		// SelectSceneはコンストラクタの処理に時間がかかるので、ローディングはここで出しておく
		co_await ShowLoadingOneFrame::Play(HasBgYN::No);
		break;

	case TitleMenuItem::kOption:
		co_await Co::ScreenFadeOut(kFadeDuration);
		requestNextScene<OptionScene>();
		break;

	case TitleMenuItem::kInputGate:
		// TODO: INPUT GATEへ遷移
		co_await Co::ScreenFadeOut(kFadeDuration);
		requestNextScene<TitleScene>(TitleMenuItem::kInputGate);
		break;

	case TitleMenuItem::kExit:
		// 効果音が途中で切れないよう終了時は少し長めにフェードアウト
		co_await Co::ScreenFadeOut(kFadeDurationExit);
		break;

	default:
		throw Error{ U"Invalid menu item: {}"_fmt(static_cast<std::underlying_type_t<TitleMenuItem>>(m_selectedMenuItem)) };
	}
}
