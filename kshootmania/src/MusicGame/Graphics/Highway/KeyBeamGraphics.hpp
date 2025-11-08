#pragma once
#include "MusicGame/GameStatus.hpp"
#include "MusicGame/ViewStatus.hpp"
#include "MusicGame/Graphics/Highway/HighwayRenderTexture.hpp"

namespace MusicGame::Graphics
{
	class KeyBeamGraphics
	{
	private:
		const Texture m_beamTexture;

	public:
		KeyBeamGraphics();

		void draw(const GameStatus& gameStatus, const ViewStatus& viewStatus, const HighwayRenderTexture& target) const;
	};
}
