#include "unknown_221810.h"

#ifdef H2_NATIVE_PORT

// Source-local ARM-safe split of the recovered out-of-line sound class lookup.
// The matching Xbox definition remains in unknown_221490.cpp, whose translation
// unit also requires unavailable Xbox DirectSound declarations.
// @retail 0x221810
s_unknown_5c *function_221810(short index)
{
	return sound_class_definition_get(index);
}

#endif
