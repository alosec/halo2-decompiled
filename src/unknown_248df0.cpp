#include "unknown_11c920.h"
#include "frame_storage.h"
#include "effects.h"

// @flags /O2 /arch:SSE /Gr

struct s_248df0
{
	byte field_00[8];
	real field_08;
	real field_0c;
	real field_10;
	word field_14[4];
	point3f field_1c;
	byte field_28[0x38 - 0x28];
	real field_38;
};


inline vector3f *function_4efb0(vector3f *arg_0);

PRIVATE __forceinline real function_248eff(real arg_0)
{
	real local_0 = arg_0 - (real)real_truncate(arg_0);
	if (local_0 < 0.f)
		local_0 += 1.f;
	return local_0;
}

PRIVATE __forceinline real function_248ef0(real const *arg_0)
{
	long local_0 = real_truncate(*arg_0);
	real local_1 = *arg_0 - (real)local_0;
	if (local_1 < 0.f)
		local_1 += 1.f;
	return local_1;
}

PRIVATE __forceinline real function_248df1(long arg_0, void *arg_1, void const *const &arg_3, void *arg_2)
{
	real local_0 = 0.f;
	s_248df0 const *local_1 = (s_248df0 const *)arg_3;
	s_particle_system_datum *local_2 = (s_particle_system_datum *)arg_1;
	switch (arg_0)
	{
	case 0:
		if (local_1) local_0 = local_1->field_08;
		break;
	case 1:
		if (local_1) local_0 = local_1->field_10;
		break;
	case 2:
	case 3:
		if (local_1) local_0 = local_1->field_14[arg_0 - 2] * (1.f / 65535.f);
		break;
	case 14:
	case 15:
		if (local_1) local_0 = local_1->field_14[arg_0 - 12] * (1.f / 65535.f);
		break;
	case 4:
		if (local_2) local_0 = local_2->unknown04;
		break;
	case 5:
	case 6:
		if (local_2) local_0 = ((real *)local_2)[arg_0 + 5];
		break;
	case 7:
		if (arg_2) local_0 = *(real *)((byte *)arg_2 + 0x1c);
		break;
	case 8:
		local_0 = (real)g_510c54->game_time * g_510c54->rate;
		break;
	case 9:
		if (local_2->effect_index != NONE)
			return ((s_effect_datum *)g_4ea93c->data)[local_2->effect_index & 0xffff].scale_a;
		return 0.f;
	case 10:
		if (local_2->effect_index != NONE)
			return ((s_effect_datum *)g_4ea93c->data)[local_2->effect_index & 0xffff].scale_b;
		return 0.f;
	case 11:
		if (local_1) local_0 = function_248ef0(&local_1->field_38);
		break;
	case 12:
		if (local_1 && arg_2)
		{
			vector3f local_3;
			local_3.i = local_1->field_1c.x - g_485618.position.x;
			local_3.j = local_1->field_1c.y - g_485618.position.y;
			local_3.k = local_1->field_1c.z - g_485618.position.z;
			function_4efb0(&local_3);
			local_0 = function_248eff((local_3.k * g_485618.forward.k + local_3.j * g_485618.forward.j + g_485618.forward.i * local_3.i) * local_1->field_38);
		}
		break;
	case 13:
		if (local_1 && arg_2)
		{
			vector3f local_3;
			volatile vector3f local_4;
			local_3.i = local_1->field_1c.x - g_485618.position.x;
			local_3.j = local_1->field_1c.y - g_485618.position.y;
			local_3.k = local_1->field_1c.z - g_485618.position.z;
			local_4.i = g_485618.up.j * g_485618.forward.k - g_485618.up.k * g_485618.forward.j;
			local_4.j = g_485618.up.k * g_485618.forward.i - g_485618.up.i * g_485618.forward.k;
			local_4.k = g_485618.up.i * g_485618.forward.j - g_485618.up.j * g_485618.forward.i;
			function_4efb0(&local_3);
			local_0 = function_248eff((local_4.k * local_3.k + local_4.j * local_3.j + local_4.i * local_3.i) * local_1->field_38);
		}
		break;
	case 16:
		if (arg_2) local_0 = *((byte *)arg_2 + 3) * (1.f / 255.f);
		break;
	}
	return local_0;
}

// @retail 0x248df0
real function_248df0(long arg_0, void *arg_1, void *arg_2, void const *arg_3)
{
	return function_248df1(arg_0, arg_1, arg_3, arg_2);
}
