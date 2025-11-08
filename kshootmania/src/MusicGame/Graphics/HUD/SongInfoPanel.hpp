#pragma once
#include "Graphics/NumberTextureFont.hpp"
#include "MusicGame/Scroll/HighwayScroll.hpp"
#include "HispeedSettingPanel.hpp"
#include "kson/ChartData.hpp"

namespace MusicGame::Graphics
{
	class SongInfoPanel
	{
	private:
		const Texture m_jacketTexture;
		const Vec2 m_jacketPosition;
		const SizeF m_scaledJacketSize;

		const RenderTexture m_titlePanelBaseTexture;
		const Vec2 m_titlePanelPosition;

		const Texture m_detailPanelBaseTexture;
		const Vec2 m_detailPanelPosition;

		const Texture m_positionMarkerTexture;

		const TiledTexture m_difficultyTexture;
		const TextureRegion m_difficultyTextureRegion;

		const int32 m_level;

		const NumberTextureFont m_numberTextureFont;
		const TextureFontTextLayout m_levelNumberLayout;
		const TextureFontTextLayout m_bpmNumberLayout;

		const HispeedSettingPanel m_hispeedSettingPanel;

	public:
		explicit SongInfoPanel(const kson::ChartData& chartData, FilePathView parentPath);

		void draw(double currentTimeSec, Duration bgmDuration, double currentBPM, const Scroll::HighwayScrollContext& highwayScrollContext, bool hasMovie) const;
	};
}
