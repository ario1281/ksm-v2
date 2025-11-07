#include "noco_utils.hpp"

namespace NocoUtils
{
	Co::Task<void> WaitForTweenByTag(std::shared_ptr<noco::Canvas> canvas, const char32* tag)
	{
		co_await Co::WaitUntil([canvas, tag]
		{
			return !canvas->isTweenPlayingByTag(tag);
		});
	}
}
