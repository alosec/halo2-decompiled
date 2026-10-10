#include "data_array.h"
#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

#include <stddef.h>

/* Exact recovered shared driver-voice record release. */

struct s_sound_voice_record
{
	short salt;
	short sound_index;
	long sound_class;
	bool ambient;
	byte unknown09;
	short reference_count;
	byte unknown0c[0x60];
};

struct s_sound_system_voice_view
{
	byte unknown00[0x70];
	dword voice_bits[4];
	dword time;
};

struct s_4e6380;
extern s_4e6380 *g_4e6380;
extern s_record_pool *g_502114;

static_assert(sizeof(s_sound_voice_record) == 0x6c,
	"driver voice record size");
static_assert(offsetof(s_sound_voice_record, sound_index) == 0x02,
	"driver voice sound index offset");
static_assert(offsetof(s_sound_voice_record, reference_count) == 0x0a,
	"driver voice reference count offset");
static_assert(offsetof(s_sound_system_voice_view, voice_bits) == 0x70,
	"sound system voice-bit offset");

// @retail 0x21e050
void sound_voice_release(long voice_index)
{
	if (voice_index != NONE)
	{
		s_sound_voice_record *voice =
			reinterpret_cast<s_sound_voice_record *>(g_502114->data) +
			(voice_index & 0xffff);
		if (--voice->reference_count == 0)
		{
			if (voice->sound_index != NONE)
			{
				dword *bits =
					reinterpret_cast<s_sound_system_voice_view *>(g_4e6380)->
						voice_bits;
				long const bit = voice->sound_index;
				bits[bit >> 5] &= ~(1u << (bit & 31));
			}
			record_pool_release(g_502114, voice_index);
		}
	}
}

#endif
