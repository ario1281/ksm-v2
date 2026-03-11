#pragma once

/// @brief コースモードで次の曲に引き継ぐ情報
struct CourseContinuation
{
	int32 gaugeValue = 0;
	int32 combo = 0;
	bool isNoError = true;
};
