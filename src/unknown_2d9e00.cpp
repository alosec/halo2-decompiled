// @flags /O2 /GL-
// Havok (third-party physics library): a buffered stream's state, from the
// stream beneath it when there is one.

#include "buffered_writer_2d9e40.h"

// @retail 0x2d9e00
bool_2d9e00 buffered_writer_2d9e40::status()
{
	return stream ? stream->status() : bool_2d9e00(size != capacity);
}
