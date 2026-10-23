#ifndef SCRATCH_2D88A0_H
#define SCRATCH_2D88A0_H
// Havok (third-party physics library): a scratch buffer shared through four
// globals, owned by a reference-counted object.
#include "reference_counted_2d88a0.h"

extern char *g_480150; // start
extern char *g_480154; // cursor
extern char *g_480158; // limit, 16 bytes before the end
extern char *g_48015c; // end

class scratch_owner_2d88a0 : public referenced_2d88a0
{
public:
	REFERENCED_NEW_2D88A0(0x16)

	scratch_owner_2d88a0()
	{
		g_480150 = 0;
		g_48015c = 0;
		g_480154 = 0;
		g_480158 = 0;
	}
	~scratch_owner_2d88a0();
};

#endif
