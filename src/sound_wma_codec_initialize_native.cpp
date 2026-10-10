// Exact compact WMA codec storage initialization isolated from the broader
// Xbox decoder implementation.  Allocation still uses the recovered staged
// physical-memory policy and the decoder itself remains a genuine boundary.

#include "unknown_11c920.h"
#include "unknown_12b400.h"
#include <stddef.h>
#include <string.h>

#ifdef H2_NATIVE_PORT

struct s_47f0d0;

struct s_wma_codec_initialize_view
{
	void *vtable;
	void *stream;
	void *decoder;
	dword position;
	dword packet_size;
	byte *buffer;
	dword packet_status[2];
};

static_assert(sizeof(s_wma_codec_initialize_view) == 0x20,
	"WMA codec size");
static_assert(offsetof(s_wma_codec_initialize_view, buffer) == 0x14,
	"WMA codec buffer offset");
static_assert(offsetof(s_wma_codec_initialize_view, packet_status) == 0x18,
	"WMA codec packet-status offset");

// @retail 0x2ae1d0
void function_2ae1d0(s_47f0d0 *codec_view)
{
	s_wma_codec_initialize_view *codec =
		reinterpret_cast<s_wma_codec_initialize_view *>(codec_view);
	long buffer = reinterpret_cast<long>(
		physical_memory_malloc_fixed(0x20000, PAGE_READWRITE));
	codec->buffer = reinterpret_cast<byte *>(buffer);
	memset(codec->packet_status, 0, sizeof(codec->packet_status));
}

#endif
