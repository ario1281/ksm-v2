#pragma once
#include "ksmaudio/ksmaudio.hpp"

enum class CommonSEType
{
	kTitleEnter,
	kSelEnter,
};

class CommonSEAddon : public IAddon
{
public:
	static constexpr StringView kAddonName = U"CommonSE";

private:
	HashTable<CommonSEType, std::unique_ptr<ksmaudio::Sample>> m_samples;

public:
	CommonSEAddon();

	virtual ~CommonSEAddon() = default;

	static void Play(CommonSEType type);
};
