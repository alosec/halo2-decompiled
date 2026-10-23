#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /arch:SSE /Gr

struct s_24de80
{
	byte field_0;
	byte field_1[3];
	long field_4;
	point3f field_8;
	real field_14;
	bool field_18;
	byte field_19;
	short field_1a;
	long field_1c;
	word field_20;
	bool field_22;
	bool field_23;
	bool field_24;
	byte field_25;
	word field_26[8];
};

struct s_24de81
{
	short field_0;
	word field_2_0 : 1;
	word : 10;
	word field_2_b : 1;
	word : 2;
	word field_2_e : 1;
	word : 1;
	byte field_4[0x16c - 4];
	long field_16c;
	long field_170;
	byte field_174[0x19c - 0x174];
	real field_19c;
	byte field_1a0[0x1ac - 0x1a0];
	short field_1ac;
	byte field_1ae[2];
	long field_1b0;
};

struct s_68a90_entry;
bool function_688c0(long player_index, s_68a90_entry *entry);

class c_24de80
{
public:
	virtual void function_24de80(short arg_0, dword *arg_1, long arg_2, s_24de80 *arg_3);
};

// @retail 0x24de80
void c_24de80::function_24de80(short arg_0, dword *arg_1, long arg_2, s_24de80 *arg_3)
{
	long local_0 = arg_0;
	s_24de81 *local_1 = 0;
	if (local_0 != NONE && local_0 >= 0 && local_0 < g_4e8c24->high_water_index)
	{
		s_24de81 *local_2 = (s_24de81 *)(g_4e8c24->data + g_4e8c24->size * local_0);
		if (local_2->field_0 != 0)
			local_1 = local_2;
	}
	dword local_3 = 0;
	if (local_1)
	{
		long local_4 = data_datum_index(g_4e8c24, local_0);
		real local_7 = local_1->field_19c;
		byte local_8 = g_4e9ae8->players[local_0].b14;
		long local_5 = (short)g_4e9ae8->players[local_0].w10;
		point3f local_6 = g_4e9ae8->players[local_0].v;
		bool local_9 = local_1->field_16c > 0;
		short local_10 = local_1->field_1ac;
		long local_11 = local_1->field_1b0 == NONE ? NONE : local_1->field_1b0 & 0xffff;
		if (*arg_1 & 1)
		{
			arg_3->field_4 = local_5;
			arg_3->field_8 = local_6;
			local_3 |= 1;
		}
		if ((*arg_1 & 2) && arg_3->field_14 != local_7)
		{
			arg_3->field_14 = local_7;
			local_3 |= 2;
		}
		if ((*arg_1 & 4) && arg_3->field_0 != local_8)
		{
			arg_3->field_0 = local_8;
			local_3 |= 4;
		}
		if ((*arg_1 & 8) && arg_3->field_18 != local_9)
		{
			arg_3->field_18 = local_9;
			local_3 |= 8;
		}
		if (*arg_1 & 0x10)
		{
			if (function_688c0(local_4, (s_68a90_entry *)arg_3->field_26))
			{
				long *local_13 = &g_4e9ae8->l6dc[local_0 * 4];
				for (long local_12 = 0; local_12 < 4; ++local_12)
					local_13[local_12] = ((long *)arg_3->field_26)[local_12];
				local_3 |= 0x10;
			}
		}
		if ((*arg_1 & 0x20) && arg_3->field_1a != local_10)
		{
			arg_3->field_1a = local_10;
			local_3 |= 0x20;
		}
		if ((*arg_1 & 0x40) && arg_3->field_1c != local_11)
		{
			arg_3->field_1c = local_11;
			local_3 |= 0x40;
		}
		if ((*arg_1 & 0x80) && arg_3->field_20 != local_1->field_170)
		{
			arg_3->field_20 = (word)local_1->field_170;
			local_3 |= 0x80;
		}
		if ((*arg_1 & 0x100) && arg_3->field_22 != local_1->field_2_b)
		{
			arg_3->field_22 = local_1->field_2_b;
			local_3 |= 0x100;
		}
		if ((*arg_1 & 0x200) && arg_3->field_23 != local_1->field_2_0)
		{
			arg_3->field_23 = local_1->field_2_0;
			local_3 |= 0x200;
		}
		if ((*arg_1 & 0x400) && arg_3->field_24 != local_1->field_2_e)
		{
			arg_3->field_24 = local_1->field_2_e;
			local_3 |= 0x400;
		}
	}
	*arg_1 = local_3;
}
