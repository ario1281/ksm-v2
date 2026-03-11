#pragma once

struct SelectFolderState
{
	enum FolderType
	{
		kNone = 0,
		kDirectory,
		kAll,
		kFavorite,
		kCourses,
	};

	enum class SortMode
	{
		kName = 0,
		kLevel,
	};

	FolderType folderType = kNone;

	SortMode sortMode = SortMode::kName;

	String fullPath;
};
