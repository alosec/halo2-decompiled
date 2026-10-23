// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_109E00.CPP: queries of g_5107f0's object list (unknown_108fd0.cpp):
   the moving objects whose velocity carries the objects attached to them */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_list.h"

struct s_object_definition_109e00
{
	word unknown00;
	word : 7;
	word flag7 : 1;
	word : 8;
};

struct s_object_109e00
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0xb8 - 0x18];
	long platform_index;
	byte unknown0bc[0xc0 - 0xbc];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word : 13;
};

struct s_object_header_109e00
{
	byte unknown00[8];
	s_object_109e00 *object;
};

#define OBJECT_GET_109e00(index) (((s_object_header_109e00 *)g_4e0300->data)[(index) & 0xffff].object)

point3f *function_b9dd0(long object_index, point3f *result);

static inline long function_x556bb4(long object_index)
{
	long parent_index = NONE;

	while (object_index != NONE)
	{
		parent_index = object_index;
		object_index = OBJECT_GET_109e00(object_index)->parent_index;
	}
	return parent_index;
}

static inline void cross3f(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}

// score 37 (was 37): the blocks are laid out in another order and the float
// registers are used in the other order.
// @retail 0x109e00
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required)
{
	s_object_109e00 *root = OBJECT_GET_109e00(function_x556bb4(object_index));
	bool result = false;

	if (TEST_FIELD_BIT(root->flag1) || TEST_FIELD_BIT(root->flag2))
	{
		long platform_index = root->platform_index;

		for (long i = 0; i < g_5107f0->object_count; i++)
		{
			if (g_5107f0->object_indices[i] == platform_index &&
				(!definition_flag_required || TEST_FIELD_BIT(((s_object_definition_109e00 *)g_4e3b44[OBJECT_GET_109e00(platform_index)->definition_index & 0xffff].bytes)->flag7)))
			{
				s_object_list_entry *entry = &g_5107f0->entries[i];

				if (entry->active)
				{
					point3f position;
					vector3f offset;

					function_b9dd0(object_index, &position);
					offset.i = position.x - entry->center.x;
					offset.j = position.y - entry->center.y;
					offset.k = position.z - entry->center.z;
					cross3f(&entry->angular_velocity, &offset, velocity);
					velocity->i = entry->linear_velocity.i + velocity->i;
					velocity->j = entry->linear_velocity.j + velocity->j;
					velocity->k = entry->linear_velocity.k + velocity->k;
					result = true;
				}
			}
		}
	}
	return result;
}

// @retail 0x109fd0
bool function_109fd0(long object_index, vector3f *velocity)
{
	long const *object_index_reference = &object_index;
	bool result = false;

	if ((*object_index_reference) != NONE)
	{
		for (long i = 0; i < g_5107f0->object_count; i++)
		{
			if (g_5107f0->object_indices[i] == (*object_index_reference))
			{
				s_object_list_entry const *entry = g_5107f0->entries + i;
				*velocity = entry->linear_velocity;
				result = true;
				break;
			}
		}
	}
	return result;
}
