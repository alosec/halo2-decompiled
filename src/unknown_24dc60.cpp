#include "unknown_11c920.h"
#include "globals.h"
#include "engine_peer.h"
#include "unknown_13927e.h"

// @flags /O2 /arch:SSE /Gr

struct s_24dc60
{
	byte field_0[0x2c];
	long field_2c;
	byte field_30[0xc0 - 0x30];
	char field_c0;
	byte field_c1[0x21c - 0xc1];
};

struct s_24dc61
{
	s_24dc60 *field_0;
	s_record_pool *field_4;
	long field_8;
	long field_c;
};

bool player_iterator_next(long *arg_0);

PRIVATE __forceinline bool function_24dc61(short arg_0, short arg_1)
{
	bool local_0 = false;
	c_engine_peer *local_1 = g_55e4d0[g_4e9ae8->engine_index];
	if (local_1)
		local_0 = local_1->p27(arg_0, arg_1);
	return local_0;
}

class c_24dc60
{
public:
	virtual void function_24dc60(long arg_0);
};

// @retail 0x24dc60
void c_24dc60::function_24dc60(long arg_0)
{
	if (arg_0 != NONE)
	{
		long local_0 = g_4e8c20->entries[arg_0];
		if (local_0 != NONE)
		{
			s_24dc60 *local_1 = (s_24dc60 *)(g_4e8c24->data + (local_0 & 0xffff) * sizeof(s_24dc60));
			if (local_1->field_2c != NONE)
			{
				s_24dc61 local_2;
				local_2.field_4 = g_4e8c24;
				local_2.field_c = NONE;
				local_2.field_8 = NONE;
				while (player_iterator_next((long *)&local_2))
				{
					if (local_2.field_8 != local_0)
					{
						if (!function_24dc61(local_2.field_0->field_c0, local_1->field_c0))
						{
							s_marker_list local_4;
							if (function_162550(local_2.field_8, &local_4))
								function_24e59f(&local_4);
						}
					}
				}
			}
		}
	}
}
