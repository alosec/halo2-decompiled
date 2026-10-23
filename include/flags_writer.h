/* FLAGS_WRITER.H: writes a block of optional fields to a bitstream: each
   field is preceded by a bit telling whether it follows, and a field that
   no longer fits is rewound (src/flags_writer.cpp) */

#ifndef FLAGS_WRITER_H
#define FLAGS_WRITER_H

#include "unknown_11c920.h"
#include "bitstream.h"

struct s_flags_writer
{
	s_bitstream *stream;
	long first_index;
	long flag_count;
	dword requested;
	dword started;
	dword written;
	dword discarded;
	dword truncated;
	bool space;
	byte unknown21[3];
	long index;
	char const *name;
	long reserve_bits;
	long reserve;
};

inline void flags_writer_initialize(s_flags_writer *writer, s_bitstream *stream, long first_index, long flag_count,
	dword requested, long reserve_bits)
{
	writer->stream = stream;
	writer->first_index = first_index;
	writer->flag_count = flag_count;
	writer->requested = requested;
	writer->reserve_bits = reserve_bits;
	writer->reserve = reserve_bits + flag_count;
	writer->started = 0;
	writer->written = 0;
	writer->discarded = 0;
	writer->truncated = 0;
	writer->index = NONE;
	writer->name = 0;
	writer->space = (stream->size_in_bytes << 3) - stream->bit_position >= writer->reserve;
}

bool flags_writer_begin(s_flags_writer *writer, long index, char const *name);
void flags_writer_end(s_flags_writer *writer);

#endif
