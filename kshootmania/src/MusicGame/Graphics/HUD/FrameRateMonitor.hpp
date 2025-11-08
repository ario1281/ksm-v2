#pragma once
#include "Graphics/NumberTextureFont.hpp"

namespace MusicGame::Graphics
{
	class FrameRateMonitor
	{
	private:
		const NumberTextureFont m_numberTextureFont;
		const TextureFontTextLayout m_numberLayout;
		const Texture m_fpsTexture;

	public:
		FrameRateMonitor();

		void draw() const;
	};
}
