#pragma once
#include <Siv3D.hpp>
#include <NocoUI.hpp>
#include <CoTaskLib.hpp>

namespace NocoUtils
{
	// 指定したタグのTweenが再生完了するまで待機
	Co::Task<void> WaitForTweenByTag(std::shared_ptr<noco::Canvas> canvas, const char32* tag);
}
