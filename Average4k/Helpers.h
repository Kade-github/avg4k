#pragma once

#include <chrono>
#include <cstdint>
#include <wtypes.h>

class Helpers {
public:

	static char encoding_table[];
	static char* decoding_table;

	static void build_decoding_table() {

		decoding_table = (char*)malloc(256);

		for (int i = 0; i < 64; i++)
			decoding_table[(unsigned char)encoding_table[i]] = i;
	}


	void base64_cleanup() {
		free(decoding_table);
	}


	static char* base64_decode(const char* data,
		size_t input_length,
		size_t* output_length) {

		if (decoding_table == NULL) build_decoding_table();

		if (input_length % 4 != 0) return NULL;

		*output_length = input_length / 4 * 3;
		if (data[input_length - 1] == '=') (*output_length)--;
		if (data[input_length - 2] == '=') (*output_length)--;

		char* decoded_data = (char*)malloc(*output_length);
		if (decoded_data == NULL) return NULL;

		for (int i = 0, j = 0; i < input_length;) {

			uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

			uint32_t triple = (sextet_a << 3 * 6)
				+ (sextet_b << 2 * 6)
				+ (sextet_c << 1 * 6)
				+ (sextet_d << 0 * 6);

			if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
			if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
			if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
		}

		return decoded_data;
	}

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

	static void GetDesktopResolution(int& horizontal, int& vertical)
	{
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)
		horizontal = desktop.right;
		vertical = desktop.bottom;
	}

	static bool is_number(const char* str)
	{
		char* endptr = 0;
		strtod(str, &endptr);

		if (*endptr != '\0' || endptr == str)
			return false;
		return true;
	}

	static void preciseSleep(double seconds) {
		using namespace std;
		using namespace std::chrono;

		static double estimate = 5e-3;
		static double mean = 5e-3;
		static double m2 = 0;
		static int64_t count = 1;

		while (seconds > estimate) {
			auto start = high_resolution_clock::now();
			Sleep(1);
			auto end = high_resolution_clock::now();

			double observed = (end - start).count() / 1e9;
			seconds -= observed;

			++count;
			double delta = observed - mean;
			mean += delta / count;
			m2 += delta * (observed - mean);
			double stddev = sqrt(m2 / (count - 1));
			estimate = mean + stddev;
		}

		// spin lock
		auto start = high_resolution_clock::now();
		while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
	}
};