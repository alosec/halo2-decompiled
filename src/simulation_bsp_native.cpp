#if H2_NATIVE_PORT

// @flags /O2 /Gr

#include "unknown_11c920.h"

// Native offline play has no replication world or entity table. BSP changes
// therefore cannot create the retail mode-5 simulation entity associated
// with a scenario slot. This is deliberately idle rather than fabricating a
// replication identifier or touching an unowned world database.
// @retail 0xa8e90
void function_a8e90(long slot)
{
	(void)slot;
}

#endif
