#pragma once
#include "MusicGame/GameStatus.hpp"

namespace MusicGame::Graphics
{
	class AchievementPanel
	{
	private:
		const TiledTexture m_texture;

	public:
		AchievementPanel();

		void draw(const GameStatus& gameStatus) const;
	};
}
