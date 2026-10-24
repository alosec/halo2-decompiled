// @flags /O2 /Ob1 /Gr
/* a creature (object type 12) that damage reached (retail 0x119280, called
   from 0xd9490 with the object index in eax, the accumulator's flags on the
   stack and the damage report in ebx): with flag 1 the creature's timer at
   +0x206 gets a random time between its tag's two seconds values (+0x178,
   +0x17c, in ticks), and a creature with an ai (+0x134 not NONE) is told of
   the damage effect's ai value (+0x12 of the report's definition, 0x211600).

   Wired through src/damage.cpp (H2_WHOLE_IMAGE): retail reads the report from ebx, a register
   argument that src's declaration of function_119280 (src/damage.cpp,
   function_d9490) and the placeholder (src/stubs/damage.cpp) leave out. The
   two-argument call cannot reach the report, so this takes it as a third
   argument; to wire it, d9490 declares and calls
   function_119280(object_index, accumulator->flags, report) and the tag
   retail tag for 0x119280 goes above the definition below. (retail also loads
   the report's +0x10 into edi for 0x211600, which reads nothing from it.) */

#include "unknown_11c920.h"
#include "globals.h"

struct s_damage_report;

void function_211600(long arg_0, long arg_1);

/* fld/fistp: the x87's default rounding, to nearest with ties to even */
static long round_to_nearest_even(real value)
{
	long result = (long)value;
	real fraction = value - (real)result;
	if (fraction > 0.5f || (fraction == 0.5f && (result & 1)))
		++result;
	else if (fraction < -0.5f || (fraction == -0.5f && (result & 1)))
		--result;
	return result;
}

/* retail 0x119280 */
// @retail 0x119280
void function_119280(long object_index, dword flags, s_damage_report const *report)
{
	byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
	if (!(flags & 1))
		return;

	byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
	real lower = *(real *)(definition + 0x178);
	real upper = *(real *)(definition + 0x17c);
	if ((lower > upper ? lower : upper) > 0.0f)
	{
		dword *seed = &g_4e7408->unknown0;
		*seed = *seed * 0x19660d + 0x3c6ef35f;
		real random = (real)(*seed >> 16) * (1.f / 65535.f);
		real ticks = (lower + (upper - lower) * random) * (real)(long)g_510c54->field_2_3;
		*(short *)(object + 0x206) = (short)round_to_nearest_even(ticks);
	}

	if (*(long *)(object + 0x134) != NONE)
	{
		byte *damage = g_4e3b44[*(long const *)((byte const *)report + 8) & 0xffff].bytes;
		function_211600(object_index, *(word *)(damage + 0x12));
	}
}
