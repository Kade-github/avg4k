#include "SkinUtils.h"

#include "Average4K.h"

Average4k::Audio::TimeSegment Average4k::Utils::SkinUtils::GetMenuThemeTiming(float bpm)
{
	
	
	Audio::TimeSegment seg = { 0, bpm, 0,static_cast<float>(INT_MAX),0,static_cast<double>(INT_MAX)};
	return seg;
}
