#pragma once
#include "music_game/game_status.hpp"
#include "kson/chart_data.hpp"

namespace MusicGame::Graphics
{
	class LaserApproachIndicator
	{
	private:
		struct LaserIndicatorState
		{
			// 表示開始時刻(楽曲時間、秒)
			double startTimeSec = kPastTimeSec;

			// 表示中かどうか
			bool isActive = false;
		};

		// テクスチャ
		const TiledTexture m_texture;
		const TextureRegion m_textureRegionNormalL;
		const TextureRegion m_textureRegionBlinkL;
		const TextureRegion m_textureRegionNormalR;
		const TextureRegion m_textureRegionBlinkR;

		const kson::ChartData& m_chartData;

		// 各LASERレーンの状態
		std::array<LaserIndicatorState, kson::kNumLaserLanesSZ> m_states;

		// トリガー済みのLASERセクション(重複トリガー防止用)
		std::array<Optional<kson::Pulse>, kson::kNumLaserLanesSZ> m_triggeredSections;

	public:
		explicit LaserApproachIndicator(const kson::ChartData& chartData);

		void update(const GameStatus& gameStatus, const kson::TimingCache& timingCache);

		void draw(double currentTimeSec) const;
	};
}
