// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0F5DC0.CPP: an object angle test (an outside function lane A's
   script evaluator 0x2a60f0 needs) */

#include "unknown_11c920.h"
#include "globals.h"
#include <math.h>

struct s_f5dc0_object
{
	byte unknown00[0x84];
	real value84;
};

struct s_f5dc0_object_header
{
	byte unknown00[8];
	s_f5dc0_object *object;
};

struct s_f5dc0_settings
{
	byte unknown00[0x70];
	real angle;
};

/* g_4e034c (globals.h) holds these settings at +0xf4 */
struct s_f5dc0_globals_view
{
	byte unknown00[0xf4];
	s_f5dc0_settings *settings;
};

__declspec(noinline)
// @retail 0xf5dc0
bool function_f5dc0(long object_index)
{
	s_f5dc0_object *object = ((s_f5dc0_object_header *)g_4e0300->data)[object_index & 0xffff].object;

	if (object->value84 < cos(1.5707964f - ((s_f5dc0_globals_view *)g_4e034c)->settings->angle))
		return true;
	return false;
}
