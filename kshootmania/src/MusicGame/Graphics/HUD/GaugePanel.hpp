#pragma once
#include "MusicGame/GameDefines.hpp"
#include "Graphics/NumberTextureFont.hpp"

namespace MusicGame::Graphics
{
	class GaugePanel
	{
	private:
		const GaugeCalcType m_gaugeCalcType;
		const TiledTexture m_baseTexture;
		const TiledTexture m_barTexture;
		const TiledTexture m_barAnimTexture;
		const Texture m_percentBaseTexture;
		const NumberTextureFont m_percentNumberTextureFont;
		const TextureFontTextLayout m_percentNumberLayout;

	public:
		explicit GaugePanel(GaugeCalcType gaugeCalcType);

		void draw(double percent, kson::Pulse currentPulse) const;
	};
}
