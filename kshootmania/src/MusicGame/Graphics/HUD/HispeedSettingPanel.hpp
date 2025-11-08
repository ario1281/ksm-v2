#pragma once
#include "Graphics/NumberTextureFont.hpp"
#include "MusicGame/Scroll/HighwayScroll.hpp"

namespace MusicGame::Graphics
{
	class HispeedSettingPanel
	{
	private:
		const NumberTextureFont m_targetValueNumberTextureFont;
		const NumberTextureFont m_currentValueNumberTextureFont;
		const TiledTexture m_hispeedLetterTexture;
		const TextureFontTextLayout m_numberLayout;
		const TextureFontTextLayout m_numberLayoutHalf;

	public:
		HispeedSettingPanel();

		void draw(const Vec2& position, const Scroll::HighwayScrollContext& highwayScrollContext) const;
	};
}
