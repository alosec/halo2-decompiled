// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_262A90.CPP: where a reference (function_262b40) stands, and the
   way it faces */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_20fe20.h"
#include "unknown_2626b0.h"

real function_30bf0(vector3f *v);
bool function_29d6c0(vector3f *vector, s_reference reference);

/* called, not inlined, by both retail callers */
__declspec(noinline) bool function_262a90(s_reference reference, point3f *point, vector3f *vector);

// @retail 0x262a90
bool function_262a90(s_reference reference, point3f *point, vector3f *vector)
{
	bool result = false;
	s_type_c3b527 *node = (s_type_c3b527 *)function_262b40(reference);

	if (node)
	{
		function_210850(node, point);
		if (function_29d6c0(vector, reference))
		{
			vector->k = 0.0f;
			if (function_30bf0(vector) > 0.0f)
				result = true;
		}
	}

	return result;
}

// @retail 0x262af0
bool function_262af0(s_reference reference, point3f *point, vector3f *facing)
{
	bool result = false;
	s_type_c3b527 *node = (s_type_c3b527 *)function_262b40(reference);

	if (node)
	{
		function_210850(node, point);
		if (facing)
			*facing = *g_4687b0;
		result = true;
	}

	return result;
}
