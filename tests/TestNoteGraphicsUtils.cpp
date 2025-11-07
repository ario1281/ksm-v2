#include <catch2/catch.hpp>
#include "music_game/graphics/highway/note/note_graphics_utils.hpp"

using namespace MusicGame::Graphics::NoteGraphicsUtils;

TEST_CASE("CalcChipNoteColorIndex for default skin", "[NoteGraphicsUtils][CalcChipNoteColorIndex]")
{
	kson::BeatInfo beatInfo;

	REQUIRE(CalcChipNoteColorIndex(0, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(15, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(30, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(60, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(80, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(120, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(160, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(180, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(240, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(300, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(360, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(480, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(600, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(720, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(840, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(960, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(1920, beatInfo, NoteSkinType::kDefault) == 0);
	REQUIRE(CalcChipNoteColorIndex(2880, beatInfo, NoteSkinType::kDefault) == 0);
}

TEST_CASE("CalcChipNoteColorIndex for note skin", "[NoteGraphicsUtils][CalcChipNoteColorIndex]")
{
	kson::BeatInfo beatInfo;

	REQUIRE(CalcChipNoteColorIndex(0, beatInfo, NoteSkinType::kNote) == 1);
	REQUIRE(CalcChipNoteColorIndex(15, beatInfo, NoteSkinType::kNote) == 8);
	REQUIRE(CalcChipNoteColorIndex(30, beatInfo, NoteSkinType::kNote) == 6);
	REQUIRE(CalcChipNoteColorIndex(60, beatInfo, NoteSkinType::kNote) == 4);
	REQUIRE(CalcChipNoteColorIndex(80, beatInfo, NoteSkinType::kNote) == 3);
	REQUIRE(CalcChipNoteColorIndex(120, beatInfo, NoteSkinType::kNote) == 2);
	REQUIRE(CalcChipNoteColorIndex(160, beatInfo, NoteSkinType::kNote) == 3);
	REQUIRE(CalcChipNoteColorIndex(180, beatInfo, NoteSkinType::kNote) == 4);
	REQUIRE(CalcChipNoteColorIndex(240, beatInfo, NoteSkinType::kNote) == 1);
	REQUIRE(CalcChipNoteColorIndex(300, beatInfo, NoteSkinType::kNote) == 4);
	REQUIRE(CalcChipNoteColorIndex(360, beatInfo, NoteSkinType::kNote) == 2);
	REQUIRE(CalcChipNoteColorIndex(480, beatInfo, NoteSkinType::kNote) == 1);
	REQUIRE(CalcChipNoteColorIndex(600, beatInfo, NoteSkinType::kNote) == 2);
	REQUIRE(CalcChipNoteColorIndex(720, beatInfo, NoteSkinType::kNote) == 1);
	REQUIRE(CalcChipNoteColorIndex(840, beatInfo, NoteSkinType::kNote) == 2);
	REQUIRE(CalcChipNoteColorIndex(960, beatInfo, NoteSkinType::kNote) == 1);
	REQUIRE(CalcChipNoteColorIndex(1920, beatInfo, NoteSkinType::kNote) == 1);
	REQUIRE(CalcChipNoteColorIndex(2880, beatInfo, NoteSkinType::kNote) == 1);
}
