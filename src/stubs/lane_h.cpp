// Stubs for game functions called by lane H's code that are not decompiled yet.

#include <xtl.h>
#include <xonline.h>

struct s_player_profile;
struct s_shapes;
union point3f;
struct s_controller_event;



struct s_event;
struct s_event_response;

struct s_dialog_definition;

struct s_screen_parameters;
class c_class_1473c9;















struct s_network_session_player;



typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);

/* the zlib inflate routines below the stream setup in src/unknown_2cb5b0.cpp */
struct s_inflate_stream;
typedef unsigned long (*block_checksum)(unsigned long checksum, unsigned char const *buffer, unsigned long size);

// @stub 0x2cc9d0
long function_2cc9d0(s_inflate_stream *stream, long flush)
{
    return -2;
}

// @stub 0x2cc890
long function_2cc890(s_inflate_stream *stream)
{
    return 0;
}

// @stub 0x2cc850
long function_2cc850(s_inflate_stream *stream)
{
    return 0;
}

// @stub 0x2ce950
void *function_2ce950(s_inflate_stream *stream, block_checksum checksum, unsigned long window_size)
{
    return 0;
}

// @stub 0x2ce7b0
unsigned long function_2ce7b0(unsigned long checksum, unsigned char const *buffer, unsigned long size)
{
    return 0;
}

// @stub 0x2cb510
long __stdcall function_2cb510(unsigned char *destination, long *destination_size, unsigned char const *source, long source_size, long level, block_allocate allocate, block_free release, void *opaque)
{
    return -1;
}



