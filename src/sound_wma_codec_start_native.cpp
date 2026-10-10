// Exact dormant WMA decoder start and input-callback routing isolated from
// the broader Xbox stream implementation.  Decoder construction remains a
// genuine WMA backend operation and failure is not replaced by fake state.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

#pragma pack(push, 1)
struct WAVEFORMATEX
{
	word wFormatTag;
	word nChannels;
	dword nSamplesPerSec;
	dword nAvgBytesPerSec;
	word nBlockAlign;
	word wBitsPerSample;
	word cbSize;
};
#pragma pack(pop)

struct s_sound_stream
{
	byte unknown00;
	char state : 3;
	char channel_count : 3;
	char unknown01_6 : 2;
	char chunk_count : 3;
	char started_count : 3;
	char unknown02_6 : 2;
	byte unknown03[0x31];
};

struct XMEDIAPACKET
{
	void *pvBuffer;
	dword dwMaxSize;
	dword *pdwCompletedSize;
	dword *pdwStatus;
	void *hCompletionEvent;
	long long *prtTimestamp;
};

struct s_xmedia_object_process_view
{
	void **vtable;
};

class c_wma_codec
{
public:
	virtual void start(s_sound_stream *stream);
	virtual bool get_packet(s_sound_stream *stream, XMEDIAPACKET *packet);

	void release_decoder();
	dword read(dword offset, dword byte_count, void **data);

	s_sound_stream *m_stream;
	void *m_decoder;
	dword m_position;
	dword m_packet_size;
	byte *m_buffer;
	dword m_packet_status[2];
};

typedef dword (__stdcall *wma_read_callback)(void *context, dword offset,
	dword byte_count, void **data);

extern "C" long __stdcall WmaCreateInMemoryDecoder(
	wma_read_callback callback,
	void *context,
	dword yield_rate,
	WAVEFORMATEX *format,
	void **decoder);

static dword __stdcall wma_codec_read_callback(void *context, dword offset,
	dword byte_count, void **data)
{
	return reinterpret_cast<c_wma_codec *>(context)->read(offset, byte_count,
		data);
}

static_assert(sizeof(WAVEFORMATEX) == 0x12, "WAVEFORMATEX size");
static_assert(sizeof(XMEDIAPACKET) == 0x18, "XMEDIAPACKET size");
static_assert(sizeof(s_sound_stream) == 0x34, "sound stream size");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
static_assert(offsetof(c_wma_codec, m_stream) == 0x04,
	"WMA codec stream offset");
static_assert(offsetof(c_wma_codec, m_decoder) == 0x08,
	"WMA codec decoder offset");
#pragma clang diagnostic pop
static_assert(sizeof(c_wma_codec) == 0x20, "WMA codec size");

// @retail 0x2ae240
void c_wma_codec::start(s_sound_stream *stream)
{
	WAVEFORMATEX format;

	release_decoder();
	stream->started_count++;
	WmaCreateInMemoryDecoder(wma_codec_read_callback, this, 1, &format,
		&m_decoder);
}

// @retail 0x2ae290
bool c_wma_codec::get_packet(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	static const dword k_packet_status_pending = 0x8000000a;
	bool result = false;
	long packet_index = NONE;

	for (long index = 0; index < 2; index++)
	{
		if (m_packet_status[index] != k_packet_status_pending)
		{
			packet_index = index;
			break;
		}
	}

	if (packet_index != NONE)
	{
		dword total = 0;
		byte *buffer = m_buffer + (packet_index << 16);

		while (total < m_packet_size)
		{
			WAVEFORMATEX format;
			XMEDIAPACKET output;
			dword completed;

			output.pvBuffer = buffer + total;
			output.dwMaxSize = m_packet_size - total;
			output.pdwCompletedSize = &completed;
			output.pdwStatus = NULL;
			output.hCompletionEvent = NULL;
			output.prtTimestamp = NULL;

			typedef long (__stdcall *process_method)(
				s_xmedia_object_process_view *object,
				XMEDIAPACKET *input,
				XMEDIAPACKET *output);
			s_xmedia_object_process_view *decoder =
				reinterpret_cast<s_xmedia_object_process_view *>(m_decoder);
			process_method process =
				reinterpret_cast<process_method>(decoder->vtable[4]);
			if (process(decoder, NULL, &output) < 0)
				break;

			m_position += completed;
			total += completed;
			if (completed < output.dwMaxSize)
			{
				release_decoder();
				m_position = 0;
			}

			if (stream->started_count >= stream->chunk_count ||
				stream->started_count != 0)
			{
				break;
			}

			stream->started_count++;
			WmaCreateInMemoryDecoder(wma_codec_read_callback, this, 1,
				&format, &m_decoder);
		}

		result = total > 0;
		if (result)
		{
			packet->pvBuffer = buffer;
			packet->dwMaxSize = total;
			packet->pdwCompletedSize = NULL;
			packet->pdwStatus = &m_packet_status[packet_index];
			packet->hCompletionEvent = NULL;
			packet->prtTimestamp = NULL;
		}
	}

	return result;
}

#endif
