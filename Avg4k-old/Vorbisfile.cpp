#include "Vorbisfile.h"

#if defined(INTEGER_VORBIS)
#include <tremor/ivorbisfile.h>
#else
#include <vorbis/vorbisfile.h>
#include <ogg/ogg.h>
#endif

#include <cstdarg>
#include <algorithm>

static size_t
OggRageFile_read_func(void* ptr, size_t size, size_t nmemb, void* datasource)
{
	auto* f = reinterpret_cast<RageFileBasic*>(datasource);
	return f->Read(ptr, size, nmemb);
}

static int
OggRageFile_seek_func(void* datasource, ogg_int64_t offset, int whence)
{
	auto* f = reinterpret_cast<RageFileBasic*>(datasource);
	return f->Seek(static_cast<int>(offset), whence);
}

static int
OggRageFile_close_func(void* datasource)
{
	return 0;
}

static long
OggRageFile_tell_func(void* datasource)
{
	auto* f = reinterpret_cast<RageFileBasic*>(datasource);
	return f->Tell();
}

static std::string
ov_ssprintf(int err, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	va_end(va);

	std::string errstr;
	switch (err) {
		// OV_FALSE, OV_EOF, and OV_HOLE were added to this switch because
		// OV_EOF cases were being reported as unknown. -Kyz
	case OV_FALSE:
		errstr = "OV_FALSE";
		break;
	case OV_EOF:
		errstr = "OV_EOF";
		break;
	case OV_HOLE:
		errstr = "OV_HOLE";
		break;
		/* XXX: In the case of OV_EREAD, can we snoop at errno? */
	case OV_EREAD:
		errstr = "Read error";
		break;
	case OV_EFAULT:
		errstr = "Internal error";
		break;
	case OV_EIMPL:
		errstr = "Feature not implemented";
		break;
	case OV_EINVAL:
		errstr = "Invalid argument";
		break;
	case OV_ENOTVORBIS:
		errstr = "Not Vorbis data";
		break;
	case OV_EBADHEADER:
		errstr = "Invalid Vorbis bitstream header";
		break;
	case OV_EVERSION:
		errstr = "Vorbis version mismatch";
		break;
	case OV_ENOTAUDIO:
		errstr = "OV_ENOTAUDIO";
		break;
	case OV_EBADPACKET:
		errstr = "OV_EBADPACKET";
		break;
	case OV_EBADLINK:
		errstr = "Link corrupted";
		break;
	case OV_ENOSEEK:
		errstr = "Stream is not seekable";
		break;
	default:
		break;
	}

	return s;
}


int Vorbisfile::GetLength() const
{
#if defined(INTEGER_VORBIS)
	int len = ov_time_total(vf, -1);
#else
	auto len = static_cast<int>(ov_time_total(vf, -1) * 1000);
#endif

	return len;
}

int
Vorbisfile::SetPosition(int iFrame)
{
	eof = false;

	const auto sample = ogg_int64_t(iFrame);

	auto ret = ov_pcm_seek(vf, sample);
	if (ret < 0) {
		/* Returns OV_EINVAL on EOF. */
		if (ret == OV_EINVAL || ret == OV_EOF) {
			eof = true;
			return 0;
		}
		return -1;
	}
	read_offset = static_cast<int>(ov_pcm_tell(vf));

	return 1;
}

int
Vorbisfile::Read(float* buf, int iFrames)
{
	auto frames_read = 0;

	while ((iFrames != 0) && !eof) {
		const int bytes_per_frame = sizeof(float) * channels;

		auto iFramesRead = 0;

		{
			auto curofs = static_cast<int>(ov_pcm_tell(vf));
			if (curofs < read_offset) {
				/* The timestamps moved backwards.  Ignore it.  This file
				 * probably won't sync correctly. */
				read_offset = curofs;
			}
			else if (curofs > read_offset) {
				/* Our offset doesn't match.  We have a hole in the data, or
				 * corruption. If we're reading with accurate syncing, insert
				 * silence to line it up. That way, corruptions in the file
				 * won't casue desyncs. */

				 /* In bytes: */
				auto iSilentFrames = curofs - read_offset;
				iSilentFrames =
					std::min(iSilentFrames, static_cast<int>(iFrames));
				auto silence = iSilentFrames * bytes_per_frame;

				memset(buf, 0, silence);
				iFramesRead = iSilentFrames;
			}
		}

		if (iFramesRead == 0) {
			int bstream;
#if defined(INTEGER_VORBIS)
			int ret = ov_read(
				vf, (char*)buf, iFrames * channels * sizeof(int16_t), &bstream);
#else // float vorbis decoder
			float** pcm;
			int ret = ov_read_float(vf, &pcm, iFrames, &bstream);
#endif

			{
				auto vi = ov_info(vf, -1);
				
			}

			if (ret == OV_HOLE)
				continue;
			if (ret == OV_EBADLINK) {
				return -1;
			}

			if (ret == 0) {
				eof = true;
				continue;
			}

#if defined(INTEGER_VORBIS)
			if (ret > 0) {
				int iSamplesRead = ret / sizeof(int16_t);
				iFramesRead = iSamplesRead / channels;

				/* Convert in reverse, so we can do it in-place. */
				const int16_t* pIn = (int16_t*)buf;
				float* pOut = (float*)buf;
				for (int i = iSamplesRead - 1; i >= 0; --i)
					pOut[i] = pIn[i] / 32768.0f;
			}
#else
			if (ret > 0) {
				iFramesRead = ret;

				int iNumChannels = channels;
				for (auto iChannel = 0; iChannel < iNumChannels; ++iChannel) {
					const float* pChannelIn = pcm[iChannel];
					auto pChannelOut = &buf[iChannel];
					for (auto i = 0; i < iFramesRead; ++i) {
						*pChannelOut = *pChannelIn;
						++pChannelIn;
						pChannelOut += iNumChannels;
					}
				}
			}
#endif
		}

		read_offset += iFramesRead;

		buf += iFramesRead * channels;
		frames_read += iFramesRead;
		iFrames -= iFramesRead;
	}

	if (!frames_read)
		return -2;

	return frames_read;
}

int
Vorbisfile::GetSampleRate() const
{
	auto vi = ov_info(vf, -1);

	return vi->rate;
}

int
Vorbisfile::GetNextSourceFrame() const
{
	auto iFrame = static_cast<int>(ov_pcm_tell(vf));
	return iFrame;
}

Vorbisfile::Vorbisfile()
{
	vf = nullptr;
}

Vorbisfile::~Vorbisfile()
{
	if (vf)
		ov_clear(vf);
	delete vf;
}