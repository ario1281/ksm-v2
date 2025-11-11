#include "CommonSEAddon.hpp"

CommonSEAddon::CommonSEAddon()
{
	m_samples.emplace(CommonSEType::kTitleEnter, std::make_unique<ksmaudio::Sample>("se/title_enter.wav"));
	m_samples.emplace(CommonSEType::kSelEnter, std::make_unique<ksmaudio::Sample>("se/sel_enter.ogg"));
}

void CommonSEAddon::Play(CommonSEType type)
{
	if (const auto pAddon = Addon::GetAddon<CommonSEAddon>(kAddonName))
	{
		if (const auto it = pAddon->m_samples.find(type); it != pAddon->m_samples.end())
		{
			it->second->play();
		}
	}
}
