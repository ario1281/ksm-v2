#pragma once
#include <Siv3D.hpp>
#include <NocoUI.hpp>
#include <CoTaskLib.hpp>

// コース達成率更新時に表示されるNewRecordパネル
class CourseResultNewRecordPanel
{
private:
	std::shared_ptr<noco::Canvas> m_canvas;

	bool m_isVisible = false;

public:
	explicit CourseResultNewRecordPanel(std::shared_ptr<noco::Canvas> canvas);

	void setValue(int32 oldAchievementRate, int32 newAchievementRate);

	bool isVisible() const;

	void startDisplay();

	void startRedisplay();

	Co::Task<void> waitForFadeIn();

	void startFadeOut();

	Co::Task<void> waitForFadeOut();
};
