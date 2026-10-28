/* UNKNOWN_221810.H: the sound classes tag (0x5c bytes per class), found
   through the tag header globals (g_4e034c). function_221810
   (src/unknown_221490.cpp) is the out-of-line lookup; the gain bounds
   (src/unknown_218c60.cpp) inline the same lookup. */
#ifndef UNKNOWN_221810_H
#define UNKNOWN_221810_H

#include "unknown_11c920.h"
#include "globals.h"

struct s_sound_tag_data
{
	byte unknown00[4];
	byte *unknown4;
};

struct s_unknown_5c
{
	byte unknown00[0x5c];
};

static inline s_unknown_5c *sound_class_definition_get(short index)
{
	s_tag_header *header = g_4e034c->header ? g_4e034c->header_alt : NULL;
	s_sound_tag_data *table = g_4e3b44[header->datum_index & 0xffff].sound;

	return (s_unknown_5c *)table->unknown4 + index;
}

s_unknown_5c *function_221810(short index);

#endif
