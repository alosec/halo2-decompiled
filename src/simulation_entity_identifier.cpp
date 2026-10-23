// @flags /O2 /arch:SSE /Gr
/* SIMULATION_ENTITY_IDENTIFIER.CPP: writes a simulation entity identifier
   (its index in the low 10 bits, its salt in the top 4) to a bitstream */

#include "unknown_11c920.h"
#include "bitstream.h"

/* writes an entity identifier's index (10 bits) and salt (4 bits) */
// @retail 0xb5650
void simulation_write_entity_identifier(long identifier, s_bitstream *stream)
{
	long index = identifier & 0x3ff;
	byte salt = (dword)identifier >> 28;
	stream_write_checked(stream, index, 10);
	stream_write_checked(stream, (byte)salt, 4);
}
