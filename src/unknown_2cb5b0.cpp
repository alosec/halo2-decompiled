// @flags /O2 /Gr
/* UNKNOWN_2CB5B0.CPP: the inflate side of the zlib 1.1.4 copy the game links
   (built with the game's link-time code generation, so it takes its arguments
   in registers): unpacking a whole block with caller-supplied allocators, and
   setting up an inflate stream. zlib is (C) 1995-2002 Jean-loup Gailly and
   Mark Adler, under the zlib license; these bodies are rebuilt from the
   game's binary */

#include "unknown_11c920.h"
#include <stdlib.h>

typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);
typedef dword (*block_checksum)(dword checksum, byte const *buffer, dword size);

/* the private state of an inflate stream */
struct s_inflate_state
{
	long mode;
	dword sub[2];
	long raw;
	dword window_log;
	void *blocks;
};

/* the stream an inflate works on */
struct s_inflate_stream
{
	byte const *input;
	dword input_left;
	dword total_input;
	byte *output;
	dword output_size;
	dword total_output;
	char const *message;
	s_inflate_state *state;
	block_allocate allocate;
	block_free release;
	void *opaque;
	long content_kind;
	dword checksum;
	dword reserved;
};
typedef char assert_inflate_stream_size[(sizeof(s_inflate_stream) == 0x38) ? 1 : -1];
typedef char assert_inflate_state_size[(sizeof(s_inflate_state) == 0x18) ? 1 : -1];

long function_2cc9d0(s_inflate_stream *stream, long flush);
long function_2cc890(s_inflate_stream *stream);
long function_2cc850(s_inflate_stream *stream);
void *function_2ce950(s_inflate_stream *stream, block_checksum checksum, dword window_length);
dword function_2ce7b0(dword checksum, byte const *buffer, dword size);

/* the default allocator of a stream that brings none */
// @retail 0x2ce780
void *__stdcall function_2ce780(void *opaque, long count, long size)
{
	return calloc(count, size);
}

/* the default release of a stream that brings none */
// @retail 0x2ce7a0
void __stdcall function_2ce7a0(void *opaque, void *address)
{
	free(address);
}

/* sets up a stream for inflating with a window of 2^window_log bytes
   (negative: no header or checksum); 0, or a negative error */
// score 90 (was a stub): the convention is retail's (stream esi, window bits ecx, version eax,
// size on the stack), but its callees are stubs: retail passes 0x2cc890/0x2cc850 the stream in
// eax and 0x2ce950 the stream in eax and the window size in ebx.
// @retail 0x2cc8d0
long function_2cc8d0(s_inflate_stream *stream, long window_log, char const *version, long stream_size)
{
	if (version == NULL || version[0] != '1' || stream_size != sizeof(s_inflate_stream))
		return -6;
	if (stream == NULL)
		return -2;
	stream->message = NULL;
	if (stream->allocate == NULL)
	{
		stream->allocate = function_2ce780;
		stream->opaque = NULL;
	}
	if (stream->release == NULL)
		stream->release = function_2ce7a0;
	if ((stream->state = (s_inflate_state *)stream->allocate(stream->opaque, 1, sizeof(s_inflate_state))) == NULL)
		return -4;
	stream->state->blocks = NULL;
	stream->state->raw = 0;
	if (window_log < 0)
	{
		window_log = -window_log;
		stream->state->raw = 1;
	}
	if (window_log < 8 || window_log > 15)
	{
		function_2cc890(stream);
		return -2;
	}
	stream->state->window_log = window_log;
	if ((stream->state->blocks = function_2ce950(stream, stream->state->raw ? NULL : function_2ce7b0,
		(dword)1 << window_log)) == NULL)
	{
		function_2cc890(stream);
		return -4;
	}
	function_2cc850(stream);
	return 0;
}

/* inflates source into destination in one pass; destination_size is the room
   on entry and the size produced on return; 0, or a negative error */
// score 14 (was a stub): only the calls to the stubbed 0x2cc9d0/0x2cc890 differ (retail passes
// them the stream in eax; the stubs take it in ecx), and with them the tail layout.
// @retail 0x2cb5b0
long __stdcall function_2cb5b0(byte *destination, long *destination_size, byte const *source, long source_size,
	block_allocate allocate, block_free release, void *opaque)
{
	s_inflate_stream stream;
	long error;

	stream.input = source;
	stream.input_left = source_size;
	stream.output = destination;
	stream.output_size = *destination_size;
	stream.allocate = allocate;
	stream.release = release;
	stream.opaque = opaque;
	error = function_2cc8d0(&stream, 15, "1.1.4", sizeof(stream));
	if (error != 0)
		return error;
	error = function_2cc9d0(&stream, 4);
	if (error != 1)
	{
		function_2cc890(&stream);
		return error == 0 ? -5 : error;
	}
	*destination_size = stream.total_output;
	error = function_2cc890(&stream);
	return error;
}
