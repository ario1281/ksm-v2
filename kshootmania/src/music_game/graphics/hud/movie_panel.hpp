#pragma once
#include <Siv3D.hpp>

namespace MusicGame::Graphics
{
	class MoviePanel
	{
	private:
		VideoTexture m_movie;
		bool m_started = false;
		double m_movieOffsetSec = 0.0;
		double m_startTimeSec = 0.0;
		bool m_enabled = false;

		static constexpr int32 kVideoOffsetFromCenterX = -302;
		static constexpr int32 kVideoBaseY = 140;
		static constexpr int32 kVideoWidth = 160;
		static constexpr int32 kVideoHeight = 120;

		static constexpr int32 kFrameWidth = 170;
		static constexpr int32 kFrameHeight = 130;
		static constexpr double kFrameAlpha = 96.0 / 255.0;

	public:
		MoviePanel(const FilePath& moviePath, double movieOffsetSec, bool enabled);

		~MoviePanel();

		void prepare(double globalOffsetSec);

		void update(double currentTimeSec);

		void draw() const;

		[[nodiscard]]
		bool isEnabled() const;
	};
}
