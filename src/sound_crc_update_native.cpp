// ARM-safe exact CRC-32 update for the static sound arena.  The recovered
// function-local table and built flag are made explicit so extraction retains
// their complete mutable lifetime with the function.

#include "unknown_11c920.h"

#ifdef H2_NATIVE_PORT

enum
{
	k_crc_table_size = 256,
	k_crc32_polynomial = 0xedb88320
};

dword g_sound_crc_table[k_crc_table_size];
bool g_sound_crc_table_built;

// @retail 0x163ba0
__attribute__((optnone))
void function_163ba0(dword *crc_reference, void const *buffer, long buffer_size)
{
	if (!g_sound_crc_table_built)
	{
		for (short table_index = 0; table_index < k_crc_table_size; table_index++)
		{
			dword crc = table_index;
			for (short round = 0; round < 8; round++)
				crc = (crc & 1) ? ((crc >> 1) ^ k_crc32_polynomial) : (crc >> 1);
			g_sound_crc_table[table_index] = crc;
		}
		g_sound_crc_table_built = true;
	}

	dword crc = *crc_reference;
	while (buffer_size-- > 0)
	{
		dword high = (crc >> 8) & 0x00ffffff;
		dword table = g_sound_crc_table[(crc ^ *static_cast<byte const *>(buffer)) & 0xff];
		buffer = static_cast<byte const *>(buffer) + 1;
		crc = high ^ table;
	}
	*crc_reference = crc;
}

#endif
