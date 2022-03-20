#pragma once
class Helpers {
public:
	static int calculateCMODY(float BPS, float ms, float msToCompare, float pixelsPerBeat)
	{
		return (BPS * ((ms - msToCompare) / 1000)) * pixelsPerBeat;
	}

	static char asciitolower(char in) {
		if (in <= 'Z' && in >= 'A')
			return in - ('Z' - 'z');
		return in;
	}

	static void removeTrailingCharacters(std::string& str, const char charToRemove) {
		str.erase(str.find_last_not_of(charToRemove) + 1, std::string::npos);
	}

	static bool is_number(const char* str)
	{
		char* endptr = 0;
		strtod(str, &endptr);

		if (*endptr != '\0' || endptr == str)
			return false;
		return true;
	}
};