#include "SkinUtils.h"

#include "Average4K.h"

Average4k::Audio::TimeSegment Average4k::Utils::SkinUtils::GetMenuThemeTiming()
{
	External::ConfigReader c = External::ConfigReader(Average4K::skin->GetPath("Music/MenuTheme.meta"));
	Audio::TimeSegment seg = { 0, static_cast<double>(c.Float("bpm")), 0,static_cast<float>(INT_MAX),0,static_cast<double>(INT_MAX) };
	return seg;
}
