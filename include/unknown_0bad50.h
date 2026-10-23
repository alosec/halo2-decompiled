#ifndef UNKNOWN_0BAD50_H
#define UNKNOWN_0BAD50_H
#include "unknown_11c920.h"
#include "unknown_246cd0.h"

struct s_object
{
	byte unknown00[4];
	dword : 26;
	dword flag26 : 1;
	dword : 5;
	byte unknown08[0xc];
	long parent_index;
	byte unknown18[0xc0 - 0x18];
    word flags_c0;
    byte unknownc2[0x124 - 0xc2];
	short markers_size;
	short markers_offset;
};

struct s_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

struct s_object_function_ab
{
    dword flags;
    long input_name;
    long output_name;
    long enable_name;
    real threshold;
    s_tag_data curve;
    long scale_name;
};

#endif
