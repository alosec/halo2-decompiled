#include "unknown_11c920.h"
#include "effects.h"

// @flags /O2 /arch:SSE /Gr /GL-

/* Retail stores this callback in the records at 0x55edd8 and 0x55ed60.
   Its arguments and floating result use the standard callback convention. */
// @retail 0x173c30
real __stdcall function_173c30(s_particle_system_datum const *particle_system, long key)
{
	real result = 0.0f;

	switch (key)
	{
	case 0x03000577:
		result = particle_system->unknown04;
		break;
	case 0x0d00069e:
		result = particle_system->random[0];
		break;
	case 0x0d00069f:
		result = particle_system->random[1];
		break;
	}
	return result;
}
