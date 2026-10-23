// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1EFDC0.CPP: tests a point against each shape of a lookup (built
   with SSE, apart from its neighbours in unknown_1efac0.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1efac0.h"

struct s_table;
struct s_iterator
{
	s_table *table;
	signed char bytes[16];
	union
	{
		dword position;
		struct
		{
			byte first;
			byte item;
			byte sub;
			byte group;
		};
	};
	void *current;

	bool advance();
};

struct s_mix_output;
struct s_mix_source;
struct s_bsp3d;
void function_1efbd0(dword id, s_mix_output *output, s_mix_source *source, signed char *indices);
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

// @retail 0x1efdc0
bool function_1efdc0(s_lookup *lookup, const point3f *point)
{
	s_lookup *const *lookup_reference = &lookup;
	s_iterator iterator;
	function_1efbd0((dword)(*lookup_reference)->tag_b, (s_mix_output *)&iterator,
		(s_mix_source *)(*lookup_reference)->tag_a, (signed char *)(*lookup_reference)->pointer_a);
	while (iterator.advance())
	{
		const transform4x3f *matrix = (const transform4x3f *)(*lookup_reference)->pointer_b + (iterator.position & 0xff);
		point3f local;
		if (matrix->scale != 0.0f)
		{
			real x = point->x - matrix->position.x;
			real y = point->y - matrix->position.y;
			real z = point->z - matrix->position.z;
			if (matrix->scale != 1.0f)
			{
				real inverse = 1.0f / matrix->scale;
				x *= inverse;
				y *= inverse;
				z *= inverse;
			}
			local.x = matrix->forward.k * z + matrix->forward.j * y + matrix->forward.i * x;
			local.y = matrix->left.k * z + matrix->left.j * y + matrix->left.i * x;
			local.z = matrix->up.k * z + matrix->up.j * y + matrix->up.i * x;
		}
		else
		{
			local.x = 0.0f;
			local.y = 0.0f;
			local.z = 0.0f;
		}
		if (function_14a280((s_bsp3d *)iterator.current, 0, &local) == NONE)
			return true;
	}
	return false;
}
