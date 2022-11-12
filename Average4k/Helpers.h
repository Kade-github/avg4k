#pragma once
#include "Rendering.h"
#include <cstdint>
#include "picosha2.h"
#include <sys/stat.h>
#include <chrono>
#include <wtypes.h>
class Helpers {
public:

	static char encoding_table[];
	static char* decoding_table;

	static Rect rotate_point(float cx, float cy, float angle, Rect p)
	{
		float s = sin(angle * (3.14159265 / 180));
		float c = cos(angle * (3.14159265 / 180));

		// translate point back to origin:
		p.x -= cx;
		p.y -= cy;

		// rotate point
		float xnew = p.x * c - p.y * s;
		float ynew = p.x * s + p.y * c;

		// translate point back:
		p.x = xnew + cx;
		p.y = ynew + cy;
		return p;
	}

	static void rotateVerts(std::vector<GL_Vertex>& verts, float deg, Rect dstRect)
	{
		if (deg != 0)
		{
			float s = sin(deg * (3.14159265 / 180));
			float c = cos(deg * (3.14159265 / 180));
			float cx = dstRect.x + dstRect.w * 0.5f;
			float cy = dstRect.y + dstRect.h * 0.5f;

			for (GL_Vertex& vert : verts)
			{
				float tx = vert.x - cx;
				float ty = vert.y - cy;
				float rx = tx * c - ty * s;
				float ry = tx * s + ty * c;
				vert.x = rx + cx;
				vert.y = ry + cy;
			}
		}
	}

	static Rect AngOffset(float startX, float startY, float endX, float endY, float size)
	{
		float diffY = endY - startY;
		float diffX = endX - startX;

		float fAng = atan2f(diffY, diffX);

		float xxOffset = size * sin(fAng);
		float yyOffset = size * cos(fAng);
		return { xxOffset, yyOffset };
	}

	static void build_decoding_table() {

		decoding_table = (char*)malloc(256);

		for (int i = 0; i < 64; i++)
			decoding_table[(unsigned char)encoding_table[i]] = i;
	}


	void base64_cleanup() {
		free(decoding_table);
	}

	static std::wstring s2ws(const std::string& s, bool isUtf8 = true)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(isUtf8 ? CP_UTF8 : CP_ACP, 0, s.c_str(), slength, 0, 0);
		std::wstring buf;
		buf.resize(len);
		MultiByteToWideChar(isUtf8 ? CP_UTF8 : CP_ACP, 0, s.c_str(), slength,
			const_cast<wchar_t*>(buf.c_str()), len);
		return buf;
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

	static bool file_exists(const std::string& name) {
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}

	static std::string setHash(std::string file)
	{
		VM_START
			std::vector<unsigned char> hash(picosha2::k_digest_size);
		picosha2::hash256(file.begin(), file.end(), hash.begin(), hash.end());

		return picosha2::bytes_to_hex_string(hash.begin(), hash.end());
		VM_END
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