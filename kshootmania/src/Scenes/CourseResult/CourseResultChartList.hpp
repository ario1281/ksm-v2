#pragma once
#include <Siv3D.hpp>
#include <NocoUI.hpp>
#include "UI/LinearMenu.hpp"

class CoursePlayState;

class CourseResultChartList
{
private:
	static constexpr int32 kItemsPerPage = 4;

	std::shared_ptr<noco::Canvas> m_canvas;

	LinearMenu m_pageMenu;

	void refreshCanvasParams(const CoursePlayState& courseState);

public:
	CourseResultChartList(std::shared_ptr<noco::Canvas> canvas, const CoursePlayState& courseState);

	void update(const CoursePlayState& courseState);

	[[nodiscard]]
	int32 currentPage() const;

	[[nodiscard]]
	bool shouldShowArrows() const;
};
