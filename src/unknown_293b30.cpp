// @flags /O2 /Ob1 /Gr /arch:SSE
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_0259a0.h"

struct s_293b30
{
	short field_0;
	short field_2;
	long field_4;
	short field_8;
	short field_a;
	bool field_c;
	byte field_d;
	bool field_e;
	byte field_f;
	short field_10;
	word field_12;
	point3f field_14;
	real field_20;
	short field_24;
	byte field_26[2];
};

struct s_293b31
{
	short field_0;
	byte field_2[0x1a];
	real field_1c;
	byte field_20[0x12];
	short field_32;
	byte field_34[0x50];
};

struct s_293b32
{
	byte field_0[0x350];
	long field_350;
	s_293b31 *field_354;
};

struct s_293b33
{
	byte field_0[0x134];
	long field_134;
	byte field_138[2];
	short field_13a;
};

struct s_293b34
{
	long field_0;
	long field_4;
	long field_8;
	bool field_c;
	byte field_d;
	bool field_e;
	byte field_f;
	short field_10;
};

struct s_flock_member_iterator
{
	long next_object_index;
	long object_index;
};

struct s_flock_object;
extern s_record_pool *g_51ecb4;
bool function_2938c0(long arg_0);
long __stdcall function_293130(long arg_0);
void function_293a20(long arg_0);
s_flock_object *function_2955b0(s_flock_member_iterator *arg_0);
void function_294380(long arg_0, long arg_1);
void __stdcall function_b8540(long arg_0);

__forceinline short function_293b31(short arg_0, short arg_1)
{
	dword local_0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
	g_4e7408->unknown0 = local_0;
	return arg_0 + (short)(((g_4e7408->unknown0 >> 16) * (arg_1 - arg_0)) >> 16);
}

// @retail 0x293b30
void function_293b30(long arg_0)
{
	s_293b30 *local_0 = &((s_293b30 *)g_51ecb4->data)[arg_0 & 0xffff];
	s_293b32 *local_1 = (s_293b32 *)g_4e0350;
	if (local_0->field_2 >= 0 && local_0->field_2 < local_1->field_350)
	{
		s_293b31 *local_2 = &local_1->field_354[local_0->field_2];
		if (local_2->field_0 == g_4686c4)
		{
			if (function_2938c0(arg_0))
			{
				if (local_0->field_a > 0)
					local_0->field_a--;
				if (local_0->field_10 > 0 && local_0->field_8 < local_2->field_32 &&
					local_0->field_a <= 0 && local_0->field_e)
				{
					real local_3 = 1.0f;
					if (local_2->field_1c > 0.0f)
						local_3 = local_2->field_1c;
					if (function_293130(arg_0) != NONE)
					{
						long local_4 = function_1469f0(1.0f / local_3);
						short local_5 = (short)((3 * local_4) / 2);
						local_0->field_a = function_293b31((short)local_4, local_5);
					}
				}
			}
			if (local_0->field_8 > 0)
			{
				function_293a20(arg_0);
				s_flock_member_iterator local_6;
				local_6.next_object_index = ((s_293b30 *)g_51ecb4->data)[arg_0 & 0xffff].field_4;
				local_6.object_index = NONE;
				s_293b33 *local_7;
				while ((local_7 = (s_293b33 *)function_2955b0(&local_6)) != NULL)
				{
					s_293b34 *local_8 = local_7->field_134 == 1 ?
						(s_293b34 *)((byte *)local_7 + local_7->field_13a) : NULL;
					if (local_8)
					{
						if (local_8->field_c)
							function_b8540(local_6.object_index);
						else if (local_8->field_e)
						{
							function_294380(arg_0, local_6.object_index);
							local_8->field_10 = 0;
						}
						else
						{
							local_8->field_10++;
							real local_9 = (real)g_510c54->field_2_3 * 3.0f;
							long local_10;
							__asm
							{
								fld local_9
								fistp local_10
							}
							if (local_8->field_10 > local_10)
								local_8->field_c = true;
						}
					}
				}
			}
		}
		else
			local_0->field_c = false;
	}
}
