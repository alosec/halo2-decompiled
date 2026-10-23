// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include "bitstream.h"
#include <string.h>

struct s_session_browser_summary;
bool __stdcall function_64d70(s_session_browser_summary *arg_0);
void __stdcall session_description_encode(s_bitstream *arg_0, void *arg_1);

// @retail 0x738f0
long __stdcall function_738f0(volatile long arg_0, byte *volatile arg_1)
{
	long local_0 = 0;
	s_bitstream local_1;
	byte local_2[0x714];
	if (function_64d70((s_session_browser_summary *)local_2))
	{
		local_1.size_in_bytes = arg_0;
		local_1.unknown08 = 1;
		local_1.mode = 1;
		local_1.data = arg_1;
		memset(local_1.data, 0, local_1.size_in_bytes);
		local_1.bit_position = 0;
		local_1.checkpoint_count = 0;
		local_1.error = false;
		local_1.unknown2c = 0;
		local_1.unknown30 = 0;
		session_description_encode(&local_1, local_2);
		if (local_1.bit_position <= local_1.size_in_bytes * 8)
			local_0 = (local_1.bit_position + 7) / 8;
	}
	return local_0;
}
