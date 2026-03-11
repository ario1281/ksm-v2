#include "FavoriteAddDialog.hpp"
#include "Common/FsUtils.hpp"
#include "Common/Encoding.hpp"
#include "Input/Cursor/CursorInput.hpp"

namespace
{
	int32 CalculateMaxSelectableNumber()
	{
		const FilePath songsDir = FsUtils::SongsDirectoryPath();

		for (int32 i = 1; i <= 9; ++i)
		{
			const FilePath favPath = FileSystem::PathAppend(songsDir, U"Favorite{}.fav"_fmt(i));
			if (!FileSystem::Exists(favPath))
			{
				return i;
			}
		}

		return 9;
	}

	FilePath GetFavoriteFilePath(int32 favoriteNumber)
	{
		const FilePath songsDir = FsUtils::SongsDirectoryPath();
		return FileSystem::PathAppend(songsDir, U"Favorite{}.fav"_fmt(favoriteNumber));
	}
}

FavoriteAddDialog::FavoriteAddDialog(const std::shared_ptr<noco::Canvas>& canvas)
	: m_canvas(canvas)
	, m_menu(LinearMenu::CreateInfoWithCursorMinMax{
		.cursorInputCreateInfo = {
			.type = CursorInput::Type::Vertical,
			.buttonFlags = CursorButtonFlags::kArrowOrBT,
		},
		.cursorMin = 1,
		.cursorMax = 1, // show()時に動的に設定
		.cyclic = IsCyclicMenuYN::Yes,
		.defaultCursor = 1,
	})
	, m_maxSelectableNumber(1)
{
}

void FavoriteAddDialog::show()
{
	m_maxSelectableNumber = CalculateMaxSelectableNumber();
	m_menu.setCursorMax(m_maxSelectableNumber);
	m_menu.setCursor(1);

	m_isVisible = true;
	m_canvas->setParamValue(U"dialog_favoriteAddDialogVisible", true);
}

void FavoriteAddDialog::hide()
{
	m_isVisible = false;
	m_canvas->setParamValue(U"dialog_favoriteAddDialogVisible", false);
}

bool FavoriteAddDialog::isVisible() const
{
	return m_isVisible;
}

void FavoriteAddDialog::update()
{
	if (!m_isVisible)
	{
		return;
	}

	m_menu.update();

	// Canvasパラメータに反映(UIは0始まりなので-1)
	m_canvas->setParamValue(
		U"dialog_favoriteAddDialogNumberIndex",
		static_cast<double>(m_menu.cursor() - 1));
}

bool FavoriteAddDialog::addToFavorite(StringView songPath)
{
	const int32 favoriteNumber = m_menu.cursor();
	const FilePath favPath = GetFavoriteFilePath(favoriteNumber);

	// パス区切りは/に統一
	String normalizedPath{ songPath };
	normalizedPath.replace(U"\\", U"/");

	// 末尾のスラッシュを削除
	if (normalizedPath.ends_with(U'/'))
	{
		normalizedPath.pop_back();
	}

	// 既存の内容を読み込み
	Array<String> lines;
	if (FileSystem::Exists(favPath))
	{
		lines = Encoding::ReadTextFileLinesShiftJISOrUTF8BasedOnBOM(favPath);
		for (auto& line : lines)
		{
			line = line.trimmed();
			line.replace(U"\\", U"/");
			if (line.ends_with(U'/'))
			{
				line.pop_back();
			}
		}
		lines.remove_if([](const String& s) { return s.isEmpty(); });
	}

	// 重複チェック
	if (lines.contains(normalizedPath))
	{
		return false;
	}

	lines.push_back(normalizedPath);

	// UTF-8 BOM付きで保存
	TextWriter writer(favPath, TextEncoding::UTF8_WITH_BOM);
	for (const auto& line : lines)
	{
		writer.writeln(line);
	}

	return true;
}

int32 FavoriteAddDialog::selectedNumber() const
{
	return m_menu.cursor();
}
