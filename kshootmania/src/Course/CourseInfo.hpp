#pragma once

struct CourseChartEntry
{
	FilePath relativePath;
	FilePath absolutePath;
	bool exists = false;
};

struct CourseInfo
{
	String title;
	String titleImgPath;
	String information;
	String iconPath;
	FilePath filePath;
	Array<CourseChartEntry> charts;

	[[nodiscard]]
	bool isValid() const
	{
		for (const auto& chart : charts)
		{
			if (!chart.exists)
			{
				return false;
			}
		}
		return !charts.isEmpty();
	}

	[[nodiscard]]
	int32 chartCount() const
	{
		return static_cast<int32>(charts.size());
	}

	static Optional<CourseInfo> Load(FilePathView kcoFilePath);
};
