#pragma once
#include "graphics/number_texture_font.hpp"

namespace MusicGame::Graphics
{
	class ScorePanel
	{
	private:
		const Texture m_captionTexture;
		const NumberTextureFont m_numberTextureFont;
		const TextureFontTextLayout m_numberLayout;

		int32 m_targetScore = 0;
		int32 m_startScore = 0;
		int32 m_displayedScore = 0;
		Stopwatch m_animationTimer;

		static constexpr Duration kAnimationDuration = 0.2s;

	public:
		ScorePanel();

		void update(int32 score);

		void draw() const;
	};
}
