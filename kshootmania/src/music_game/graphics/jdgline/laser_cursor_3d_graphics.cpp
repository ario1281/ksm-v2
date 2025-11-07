#include "jdgoverlay_3d_graphics.hpp"
#include "music_game/graphics/graphics_defines.hpp"
#include "music_game/camera/camera_math.hpp"
#include "kson/common/common.hpp"
#include "laser_cursor_3d_graphics.hpp"

namespace MusicGame::Graphics
{
	namespace
	{
		constexpr StringView kTextureFilename = U"laser_cur.png";
		constexpr Size kSourceSize = { 64, 64 };

		constexpr Float3 kPlaneCenter = { 0.0f, 3.5f, -kHighwayPlaneSize.y / 2 - 1.8f };
		constexpr Float2 kPlaneSize = { 4.5f, 4.5f };

		Vec3 CursorVec(bool wide, double cursorX, double jdgoverlayScale)
		{
			const int32 xScale = wide ? kLaserXScaleWide : kLaserXScaleNormal;
			return { (cursorX - 0.5) * 282 / 8 * xScale * jdgoverlayScale, 0.0, 0.0 };
		}
	}

	LaserCursor3DGraphics::LaserCursor3DGraphics()
		: m_texture(kTextureFilename,
			{
				.row = kson::kNumLaserLanes,
				.sourceScale = SourceScale::kNoScaling,
				.sourceSize = kSourceSize,
			})
		, m_mesh(MeshData::Billboard())
	{
	}

	void LaserCursor3DGraphics::draw3D(const GameStatus& gameStatus, const ViewStatus& viewStatus, const BasicCamera3D& camera) const
	{
		// LASERカーソルを3D空間上に描画
		const ScopedRenderStates3D blendState(kEnableAlphaBlend);
		const double jdglineScale = viewStatus.camStatus.useLegacyJdgScale
			? Camera::LegacyJdglineScale(viewStatus.camStatus.zoomBottom)
			: Camera::JdglineScale(viewStatus.camStatus.zoomBottom);
		for (int32 i = 0; i < kson::kNumLaserLanes; ++i)
		{
			const auto& laneStatus = gameStatus.laserLaneStatus[i];
			const auto cursorXOpt = laneStatus.cursorXForDraw();
			if (!cursorXOpt.has_value())
			{
				// カーソルが出ていない
				continue;
			}

			// カーソルを描画
			const double cursorX = cursorXOpt.value();
			const double jdgoverlayScale = viewStatus.camStatus.useLegacyJdgScale
				? Camera::LegacyJdgoverlayScale(viewStatus.camStatus.zoomBottom)
				: Camera::JdgoverlayScale(viewStatus.camStatus.zoomBottom);
			const Vec3 shiftXVec = Vec3::Right(viewStatus.camStatus.shiftX);
			const Vec3 center = kPlaneCenter + CursorVec(laneStatus.cursorWide, cursorX, jdgoverlayScale) + shiftXVec * jdgoverlayScale;
			const double radians = Math::ToRadians(viewStatus.camStatus.rotationDeg + viewStatus.camStatus.rotationDegHighway) + viewStatus.tiltRadians;
			const Transformer3D transform(Mat4x4::Scale(jdglineScale, center) * TiltTransformMatrix(radians));
			m_mesh.draw(camera.billboard(center, kPlaneSize), m_texture(i));
		}
	}
}
