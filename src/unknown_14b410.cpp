// @flags /O2 /Gr
/* UNKNOWN_14B410.CPP: a 64-bit identifier made of this machine's six byte
   address (0x4cf7cc) and an index */

#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

#pragma pack(push, 4)
struct s_type_fb9815
{
	__int64 value;
	dword signature;
};
#pragma pack(pop)

// @retail 0x14b410
inline void machine_identifier_build(s_type_fb9815 *identifier, long index)
{
	memset(identifier, 0, sizeof(s_type_fb9815));
	identifier->value = (((((((__int64)g_4cf7cc[5] << 8 | (__int64)g_4cf7cc[4]) << 8 | (__int64)g_4cf7cc[3]) << 8 |
		(__int64)g_4cf7cc[2]) << 8 | (__int64)g_4cf7cc[1]) << 8 | (__int64)g_4cf7cc[0]) << 16) | (__int64)index;
	identifier->signature = 0xbad00000;
}
