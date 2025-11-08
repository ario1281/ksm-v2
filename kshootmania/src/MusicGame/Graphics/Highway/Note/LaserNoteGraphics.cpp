#include "LaserNoteGraphics.hpp"
#include "NoteGraphicsUtils.hpp"
#include "MusicGame/Graphics/GraphicsDefines.hpp"

namespace MusicGame::Graphics
{
	namespace
	{
		constexpr StringView kLaserNoteTextureFilename = U"laser.gif";
		constexpr StringView kLaserNoteMaskTextureFilename = U"laser_mask.gif";
		constexpr StringView kLaserNoteLeftStartTextureFilename = U"laserl_0.gif";
		constexpr StringView kLaserNoteRightStartTextureFilename = U"laserr_0.gif";

		constexpr Size kLaserTextureSize = { 48, 48 };
		constexpr int32 kLaserLineWidth = kLaserTextureSize.x;
		constexpr Size kLaserStartTextureSize = { 44, 200 };
		constexpr int32 kLaserShiftY = -10;
		constexpr int32 kLaserTailHeight = 80;

		constexpr double kLaserCriticalBlinkIntervalSec = 0.12;

		enum class JudgmentStatus
		{
			kNormal,
			kCritical,
			kError,
		};

		constexpr Quad LaserLineQuad(const Vec2& positionStart, const Vec2& positionEnd)
		{
			return {
				positionStart + Vec2{ kLaserLineWidth / 2, 0.0 },
				positionStart + Vec2{ -kLaserLineWidth / 2, 0.0 },
				positionEnd + Vec2{ -kLaserLineWidth / 2, 0.0 },
				positionEnd + Vec2{ kLaserLineWidth / 2, 0.0 }
			};
		}

		constexpr double LaserPointX(double v, int32 xScale)
		{
			return (v * (kHighwayTextureSize.x - kLaserLineWidth) + kLaserLineWidth / 2) * xScale;
		}

		void DrawLaserLine(int32 laneIdx, int32 positionY, const kson::GraphPoint& point, int32 nextPositionY, const kson::GraphPoint& nextPoint, const Texture& laserNoteTexture, int32 laserNoteTextureRow, int32 xScale, bool isScrollSpeedPositive)
		{
			// 現在の点が直角の場合、scroll_speedの符号に応じてY座標をオフセット
			int32 currentOffset = 0;
			if (point.v.v != point.v.vf)
			{
				currentOffset = isScrollSpeedPositive ? -kLaserTextureSize.y : kLaserTextureSize.y;
			}

			const Vec2 positionStart = {
				LaserPointX(point.v.vf, xScale),
				positionY + currentOffset
			};

			const Vec2 positionEnd = {
				LaserPointX(nextPoint.v.v, xScale),
				nextPositionY
			};

			const Quad quad = LaserLineQuad(positionStart, positionEnd);
			quad(laserNoteTexture(kLaserTextureSize.x * laneIdx, kLaserTextureSize.y * laserNoteTextureRow + kLaserTextureSize.y - 1 + kOnePixelTextureSourceOffset, kLaserTextureSize.x, kOnePixelTextureSourceSize)).draw();
		}

		constexpr Quad LaserSlamLineQuad(const Vec2& positionStart, const Vec2& positionEnd, bool isScrollSpeedPositive)
		{
			if (Abs(positionEnd.x - positionStart.x) <= kLaserLineWidth)
			{
				// 左右が近すぎて直角レーザーの横線が描画できない場合はゼロを返す
				return { Vec2::Zero(), Vec2::Zero(), Vec2::Zero(), Vec2::Zero() };
			}

			const int32 diffXSign = Sign(positionEnd.x - positionStart.x);

			if (isScrollSpeedPositive)
			{
				// scroll_speedが正の場合、奥方向(上)に太さを持つ
				return {
					positionStart + Vec2{ diffXSign * kLaserLineWidth / 2, -kLaserLineWidth },
					positionEnd + Vec2{ -diffXSign * kLaserLineWidth / 2, -kLaserLineWidth },
					positionEnd + Vec2{ -diffXSign * kLaserLineWidth / 2, 0 },
					positionStart + Vec2{ diffXSign * kLaserLineWidth / 2, 0 }
				};
			}
			else
			{
				// scroll_speedが負の場合、手前方向(下)に太さを持つ
				return {
					positionStart + Vec2{ diffXSign * kLaserLineWidth / 2, 0 },
					positionEnd + Vec2{ -diffXSign * kLaserLineWidth / 2, 0 },
					positionEnd + Vec2{ -diffXSign * kLaserLineWidth / 2, kLaserLineWidth },
					positionStart + Vec2{ diffXSign * kLaserLineWidth / 2, kLaserLineWidth }
				};
			}
		}

		void DrawLaserSlam(int32 laneIdx, int32 positionY, const kson::GraphPoint& point, const Texture& laserNoteTexture, int32 laserNoteTextureRow, int32 xScale, bool isScrollSpeedPositive)
		{
			const Vec2 positionStart = {
				LaserPointX(point.v.v, xScale),
				positionY
			};

			const Vec2 positionEnd = {
				LaserPointX(point.v.vf, xScale),
				positionY
			};

			// 直角レーザーの角のテクスチャを描画
			const bool isLeftToRight = (point.v.v < point.v.vf);
			const int32 yOffset = isScrollSpeedPositive ? -kLaserLineWidth / 2 : kLaserLineWidth / 2;

			if (isScrollSpeedPositive)
			{
				// scroll_speedが正の場合、奥方向(上)に太さを持つ
				laserNoteTexture(kLaserTextureSize.x * laneIdx, kLaserTextureSize.y * laserNoteTextureRow, kLaserTextureSize).mirrored(isLeftToRight).drawAt(positionStart + Vec2{ 0, yOffset });
				laserNoteTexture(kLaserTextureSize.x * laneIdx, kLaserTextureSize.y * laserNoteTextureRow, kLaserTextureSize).mirrored(!isLeftToRight).flipped().drawAt(positionEnd + Vec2{ 0, yOffset });
			}
			else
			{
				// scroll_speedが負の場合、手前方向(下)に太さを持つ(上下反転)
				laserNoteTexture(kLaserTextureSize.x * laneIdx, kLaserTextureSize.y * laserNoteTextureRow, kLaserTextureSize).mirrored(isLeftToRight).flipped().drawAt(positionStart + Vec2{ 0, yOffset });
				laserNoteTexture(kLaserTextureSize.x * laneIdx, kLaserTextureSize.y * laserNoteTextureRow, kLaserTextureSize).mirrored(!isLeftToRight).drawAt(positionEnd + Vec2{ 0, yOffset });
			}

			// 直角レーザーの横線を描画
			const Quad quad = LaserSlamLineQuad(positionStart, positionEnd, isScrollSpeedPositive);
			quad(laserNoteTexture(kLaserTextureSize.x * laneIdx + kOnePixelTextureSourceOffset, kLaserTextureSize.y * laserNoteTextureRow, kOnePixelTextureSourceSize, kLaserTextureSize.y)).draw();
		}

		void DrawLaserSlamTail(int32 laneIdx, int32 positionY, const kson::GraphPoint& point, const Texture& laserNoteTexture, int32 laserNoteTextureRow, int32 xScale, bool isScrollSpeedPositive)
		{
			const Vec2 positionStart = {
				LaserPointX(point.v.vf, xScale),
				positionY
			};

			// scroll_speedが正の場合は上方向、負の場合は下方向にtailを描画
			const Quad quad = isScrollSpeedPositive
				? LaserLineQuad(positionStart + Vec2{ 0.0, -kLaserTextureSize.y }, positionStart + Vec2{ 0.0, -kLaserTextureSize.y - kLaserTailHeight })
				: LaserLineQuad(positionStart + Vec2{ 0.0, kLaserTextureSize.y }, positionStart + Vec2{ 0.0, kLaserTextureSize.y + kLaserTailHeight });
			quad(laserNoteTexture(kLaserTextureSize.x * laneIdx, kLaserTextureSize.y * laserNoteTextureRow + kLaserTextureSize.y - 1 + kOnePixelTextureSourceOffset, kLaserTextureSize.x, kOnePixelTextureSourceSize)).draw();
		}

		JudgmentStatus GetLaserSectionJudgmentStatus(const LaserLaneStatus& laneStatus, kson::Pulse laserSectionY)
		{
			if (laneStatus.currentLaserSectionPulse != laserSectionY)
			{
				// 現在判定対象になっていなければNormal
				return JudgmentStatus::kNormal;
			}

			if (laneStatus.isCursorInCriticalJudgmentRange())
			{
				// カーソルがクリティカル判定の範囲内であればCritical
				return JudgmentStatus::kCritical;
			}

			// 判定が外れていればError
			return JudgmentStatus::kError;
		}

		void DrawLaserSection(int32 laneIdx, kson::Pulse y, const kson::LaserSection& laserSection, const Scroll::HighwayScrollContext& highwayScrollContext, const RenderTexture& target, const Texture& laserNoteTexture, int32 laserNoteTextureRow, const TextureRegion& laserStartTexture)
		{
			const ScopedRenderTarget2D renderTarget(target);

			// はみ出しLASERの場合は描画領域を横幅2倍として扱う
			const bool wide = laserSection.wide();
			const int32 xScale = wide ? kLaserXScaleWide : kLaserXScaleNormal;
			const Transformer2D transformer(Mat3x2::Translate(wide ? kLaserPositionOffsetWide : kLaserPositionOffsetNormal));

			// scroll_speedに負の値が含まれている場合は過去の点も描画範囲に入る可能性がある
			const bool hasNegativeScrollSpeed = highwayScrollContext.hasNegativeScrollSpeed();

			// LASERセクション内の各点をもとに描画
			for (auto itr = laserSection.v.begin(); itr != laserSection.v.end(); ++itr)
			{
				const auto& [ry, point] = *itr;

				const int32 positionY = highwayScrollContext.getPositionY(y + ry) + kLaserShiftY;

				// 現在の点が描画範囲内かチェック
				const bool isInRange = positionY >= 0 && positionY < kHighwayTextureSize.y;

				// この点のPulse位置でのscroll_speedが正かを取得
				const bool isScrollSpeedPositive = highwayScrollContext.isScrollSpeedPositiveAt(y + ry);

				// レーザー開始テクスチャを描画
				if (itr == laserSection.v.begin())
				{
					// 開始テクスチャの描画範囲を計算
					const int32 startTextureMinY = isScrollSpeedPositive ? positionY : positionY - kLaserStartTextureSize.y;
					const int32 startTextureMaxY = isScrollSpeedPositive ? positionY + kLaserStartTextureSize.y : positionY;
					const bool isStartTextureInRange = startTextureMaxY >= 0 && startTextureMinY < kHighwayTextureSize.y;

					if (isStartTextureInRange)
					{
						const Vec2 positionStart = {
							LaserPointX(point.v.v, xScale),
							positionY
						};

						// scroll_speedが正の場合は下方向、負の場合は上方向にテクスチャを配置
						if (isScrollSpeedPositive)
						{
							laserStartTexture.draw(Arg::topCenter = positionStart);
						}
						else
						{
							laserStartTexture.flipped().draw(Arg::bottomCenter = positionStart);
						}
					}
				}

				// 直角レーザーを描画
				if (isInRange && point.v.v != point.v.vf)
				{
					DrawLaserSlam(laneIdx, positionY, point, laserNoteTexture, laserNoteTextureRow, xScale, isScrollSpeedPositive);
				}

				// レーザー終端の点の場合は線を描画しない
				const auto nextItr = std::next(itr);
				if (nextItr == laserSection.v.end())
				{
					// 終端が直角の場合は終端を伸ばす
					if (point.v.v != point.v.vf)
					{
						// tailの描画範囲を計算
						const int32 tailMinY = isScrollSpeedPositive ? positionY - kLaserTextureSize.y - kLaserTailHeight : positionY + kLaserTextureSize.y;
						const int32 tailMaxY = isScrollSpeedPositive ? positionY - kLaserTextureSize.y : positionY + kLaserTextureSize.y + kLaserTailHeight;
						const bool isTailInRange = tailMaxY >= 0 && tailMinY < kHighwayTextureSize.y;

						if (isTailInRange)
						{
							DrawLaserSlamTail(laneIdx, positionY, point, laserNoteTexture, laserNoteTextureRow, xScale, isScrollSpeedPositive);
						}
					}

					break;
				}

				// レーザーの2つの点をもとに線を描画
				{
					const auto& [nextRy, nextPoint] = *nextItr;
					const int32 nextPositionY = highwayScrollContext.getPositionY(y + nextRy) + kLaserShiftY;

					// scroll_speedが負の場合、始点と終点が逆転する可能性があるため両方チェック
					const int32 minY = Min(positionY, nextPositionY);
					const int32 maxY = Max(positionY, nextPositionY);

					// 線の両端点が完全に描画範囲外の場合はスキップ
					if (maxY < 0 || minY >= kHighwayTextureSize.y)
					{
						// scroll_speedに負の値がなく、線全体が上にある場合は以降も描画対象外
						if (!hasNegativeScrollSpeed && maxY < 0)
						{
							return;
						}
						continue;
					}

					DrawLaserLine(laneIdx, positionY, point, nextPositionY, nextPoint, laserNoteTexture, laserNoteTextureRow, xScale, isScrollSpeedPositive);
				}
			}
		}

		int32 LaserTextureRow(JudgmentStatus judgmentStatus, double currentTimeSec)
		{
			switch (judgmentStatus)
			{
			case JudgmentStatus::kCritical:
				return MathUtils::WrappedFmod(currentTimeSec, kLaserCriticalBlinkIntervalSec) < kLaserCriticalBlinkIntervalSec / 2 ? 2 : 1;

			case JudgmentStatus::kError:
				return 3;

			case JudgmentStatus::kNormal:
			default:
				return 0;
			}
		}
	}

	LaserNoteGraphics::LaserNoteGraphics()
		: m_laserNoteTexture(TextureAsset(kLaserNoteTextureFilename))
		, m_laserNoteMaskTexture(TextureAsset(kLaserNoteMaskTextureFilename))
		, m_laserNoteStartTextures{
			TiledTexture(
				kLaserNoteLeftStartTextureFilename,
				{
					.column = kNumTextureColumnsMainSub,
					.sourceSize = kLaserStartTextureSize,
				}),
			TiledTexture(
				kLaserNoteRightStartTextureFilename,
				{
					.column = kNumTextureColumnsMainSub,
					.sourceSize = kLaserStartTextureSize,
				}) }
	{
	}

	void LaserNoteGraphics::draw(const kson::ChartData& chartData, const PlayOption& playOption, const GameStatus& gameStatus, const Scroll::HighwayScrollContext& highwayScrollContext, const HighwayRenderTexture& target) const
	{
		const ScopedRenderStates2D samplerState(SamplerState::ClampNearest);
		const ScopedRenderStates2D renderState(BlendState::Additive);

		// LASERノーツを描画
		for (int32 laneIdx = 0; laneIdx < kson::kNumLaserLanes; ++laneIdx) // 座標計算で結局int32にする必要があるのでここではsize_t不使用
		{
			const auto& lane = chartData.note.laser[laneIdx];
			const auto& laneStatus = gameStatus.laserLaneStatus[laneIdx];

			// scroll_speedに負の値が含まれている場合は過去のノーツも描画範囲に入る可能性があるため、先頭から走査
			const bool hasNegativeScrollSpeed = highwayScrollContext.hasNegativeScrollSpeed();
			auto itr = hasNegativeScrollSpeed ? lane.begin() : kson::ValueItrAt(lane, gameStatus.currentPulse);

			for (; itr != lane.end(); ++itr)
			{
				const auto& [y, laserSection] = *itr;

				const kson::RelPulse lengthRy = laserSection.v.rbegin()->first;
				const int32 sectionStartPositionY = highwayScrollContext.getPositionY(y) + kLaserShiftY;
				const int32 sectionEndPositionY = highwayScrollContext.getPositionY(y + lengthRy) + kLaserShiftY;

				// セクション開始位置と終了位置でのscroll_speedの符号を取得
				const bool isStartScrollSpeedPositive = highwayScrollContext.isScrollSpeedPositiveAt(y);
				const bool isEndScrollSpeedPositive = highwayScrollContext.isScrollSpeedPositiveAt(y + lengthRy);

				// 開始テクスチャの描画範囲を計算
				const int32 startTextureMinY = isStartScrollSpeedPositive ? sectionStartPositionY : sectionStartPositionY - kLaserStartTextureSize.y;
				const int32 startTextureMaxY = isStartScrollSpeedPositive ? sectionStartPositionY + kLaserStartTextureSize.y : sectionStartPositionY;

				// 終端が直角の場合のtailの描画範囲を計算
				const bool isEndSlam = (laserSection.v.rbegin()->second.v.v != laserSection.v.rbegin()->second.v.vf);
				int32 tailMinY = sectionEndPositionY;
				int32 tailMaxY = sectionEndPositionY;
				if (isEndSlam)
				{
					tailMinY = isEndScrollSpeedPositive ? sectionEndPositionY - kLaserTextureSize.y - kLaserTailHeight : sectionEndPositionY + kLaserTextureSize.y;
					tailMaxY = isEndScrollSpeedPositive ? sectionEndPositionY - kLaserTextureSize.y : sectionEndPositionY + kLaserTextureSize.y + kLaserTailHeight;
				}

				// セクション全体の描画範囲を計算
				const int32 minY = Min(sectionStartPositionY, Min(sectionEndPositionY, Min(startTextureMinY, tailMinY)));
				const int32 maxY = Max(sectionStartPositionY, Max(sectionEndPositionY, Max(startTextureMaxY, tailMaxY)));

				// LASERセクション全体が描画範囲外の場合はスキップ
				if (maxY < 0 || minY >= kHighwayTextureSize.y)
				{
					if (!hasNegativeScrollSpeed && maxY < 0)
					{
						// scroll_speedに負の値がなく、セクション全体が上にある場合はループを抜ける
						break;
					}
					continue;
				}

				// LASERセクションの判定状況をもとに描画すべきテクスチャの行を取得
				// Offモードの場合は常に通常状態のUVを使用
				const JudgmentStatus judgmentStatus = (playOption.laserJudgmentPlayMode == JudgmentPlayMode::kOff) ? JudgmentStatus::kNormal : GetLaserSectionJudgmentStatus(laneStatus, y);
				const int32 textureRow = LaserTextureRow(judgmentStatus, gameStatus.currentTimeSec);

				// LASERセクションを描画
				DrawLaserSection(laneIdx, y, laserSection, highwayScrollContext, target.additiveTexture(), m_laserNoteTexture, textureRow, m_laserNoteStartTextures[laneIdx](0, kTextureColumnMain));
				DrawLaserSection(laneIdx, y, laserSection, highwayScrollContext, target.invMultiplyTexture(), m_laserNoteMaskTexture, textureRow, m_laserNoteStartTextures[laneIdx](0, kTextureColumnSub));
			}
		}
	}
}
