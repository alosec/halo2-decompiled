// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"

struct s_fog_state;
byte *function_1318a0(long index);

struct s_type_12fea0_view
{
	byte field_0[0x9c];
	long field_9c;
	color3f field_a0;
	real field_ac;
	real field_b0;
	real field_b4;
	real field_b8;
	real field_bc;
	bool field_c0;
	byte field_c1[3];
	real field_c4;
	color3f field_c8;
	real field_d4;
	real field_d8;
	real field_dc;
	byte field_e0[0x14];
	plane3f field_f4;
	bool field_104;
};

struct s_type_12fea0_cluster
{
	byte field_0[0x70];
	char field_70;
	char field_71;
	byte field_72[0xb0 - 0x72];
};

struct s_type_12fea0_plane
{
	short field_0;
	short field_2;
	plane3f field_4;
	long field_14;
};

struct s_type_12fea0_bsp
{
	byte field_0[0x68];
	s_type_12fea0_plane *field_68;
	byte field_6c[0xa0 - 0x6c];
	s_type_12fea0_cluster *field_a0;
};

struct s_type_12fea0_scenario
{
	byte field_0[0x348];
	long field_348;
	byte *field_34c;
};

struct s_type_12fea0_definition
{
	word field_0;
	byte field_2[0xc - 2];
	real field_c;
	real field_10;
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	color3f field_24;
};

#pragma inline_depth(0)
// @retail 0x12fea0
bool function_12fea0(s_fog_state *state, long cluster_index, point3f const *point, vector3f const *normal)
{
	bool result = false;
	s_type_12fea0_view *fog = (s_type_12fea0_view *)state;
	s_type_12fea0_cluster *cluster = &((s_type_12fea0_bsp *)g_4e0348)->field_a0[cluster_index];
	fog->field_f4.n = *normal;
	plane3f *local_0 = &fog->field_f4;
	real local_1 = local_0->k * point->z;
	local_1 += local_0->j * point->y;
	local_1 += point->x * local_0->i;
	local_0->d = local_1;
	{
		long definition_index;
		if (cluster->field_71 != NONE)
		{
			s_type_12fea0_plane *local_plane = &((s_type_12fea0_bsp *)g_4e0348)->field_68[cluster->field_71];
			s_type_12fea0_scenario *scenario = (s_type_12fea0_scenario *)g_4e0350;
			long local_index = local_plane->field_0;
			byte *local_entry = NULL;
			if (scenario && local_index >= 0 && local_index < scenario->field_348)
				local_entry = scenario->field_34c + local_index * 16;
			if (!local_entry)
				goto done;
			definition_index = *(long *)(local_entry + 8);
			fog->field_f4 = local_plane->field_4;
			fog->field_104 = true;
		}
		else
		{
			if (cluster->field_70 == NONE || ((byte)cluster->field_70 & 0x80))
				goto done;
			byte *local_entry = function_1318a0(cluster->field_70 & 0x7f);
			if (!local_entry)
				goto done;
			definition_index = *(long *)(local_entry + 8);
			result = true;
		}
		if (definition_index != NONE)
		{
			s_type_12fea0_definition *definition = (s_type_12fea0_definition *)g_4e3b44[definition_index & 0xffff].bytes;
			bool local_inside = result;
			fog->field_c0 = true;
			if (fog->field_104)
			{
				real local_2 = local_0->k * point->z;
				local_2 += local_0->j * point->y;
				local_2 += point->x * local_0->i;
				local_2 -= local_0->d;
				if (local_2 < 0.0f)
					local_inside = true;
			}
			if ((definition->field_0 & 0x10) || ((definition->field_0 & 0x20) && !local_inside))
				goto done;
			fog->field_9c = definition_index;
			fog->field_a0 = definition->field_24;
			fog->field_b0 = definition->field_10;
			fog->field_b4 = definition->field_14;
			fog->field_ac = definition->field_c;
			fog->field_b8 = definition->field_18;
			fog->field_bc = definition->field_1c;
			fog->field_c4 = definition->field_20 < -1.0f ? -1.0f : definition->field_20 > 1.0f ? 1.0f : definition->field_20;
			if (!fog->field_104)
			{
				fog->field_f4.n = *normal;
				fog->field_f4.d = fog->field_b0 > fog->field_b4 ? fog->field_b0 : fog->field_b4;
				fog->field_104 = true;
			}
			if (definition->field_1c > definition->field_18)
				fog->field_c0 = false;
		}
	}
done:
	real local_3 = fog->field_dc;
	if (local_3 > 0.0f)
	{
		fog->field_a0.red += (fog->field_c8.red - fog->field_a0.red) * local_3;
		fog->field_a0.green += (fog->field_c8.green - fog->field_a0.green) * local_3;
		fog->field_a0.blue += (fog->field_c8.blue - fog->field_a0.blue) * local_3;
		fog->field_ac += (fog->field_d4 - fog->field_ac) * local_3;
		fog->field_104 = true;
	}
	return result;
}
#pragma inline_depth(255)
