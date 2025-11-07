#pragma once
#include "music_game/game_status.hpp"

namespace MusicGame::Graphics
{
	namespace NoteGraphicsUtils
	{
		TiledTexture ApplyAlphaToNoteTexture(const Texture& texture, const TiledTextureSizeInfo& sizeInfo);

		int32 ChipNoteHeight(double yRate);

		int32 CalcChipNoteColorIndex(kson::Pulse y, const kson::BeatInfo& beatInfo, NoteSkinType noteSkin);
	}
}
