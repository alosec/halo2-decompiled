#ifndef FONT_LOADING_H
#define FONT_LOADING_H

#include "unknown_11c920.h"
#include "job_queue.h"

/* the font headers (src/font_loading.cpp) */

struct s_kerning_pair
{
	byte first_character;
	byte second_character;
	short offset;
};

struct s_font_header
{
	long version;
	short ascending_height;
	short descending_height;
	short leading_height;
	byte unknown0a[0x16];
	long kerning_pair_count;
	s_kerning_pair kerning_pairs[96];
	dword kerning_characters[8];
};

#define k_maximum_font_count 10

/* the font cache: one entry (0x1d0 bytes) per font file, with its header
   and its open file (src/unknown_1406b0.cpp reads the characters from it) */
struct s_font_cache_entry
{
	s_font_header header;
	s_file_handle file;
	bool volatile done;
	bool pending;
	long task;
};

/* the font indices of the font table */
extern long g_4e28f4[11];

s_font_header *font_get(long font_index);

#include "font_loading_storage.h"
#endif
