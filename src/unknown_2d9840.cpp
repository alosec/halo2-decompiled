// @flags /O2 /GL-
// Havok (third-party physics library): a method that always returns 1 and
// ignores its two arguments.

#include "buffered_writer_2d9e40.h"

// @retail 0x2d9840
long stream_writer_2d9e40::seek(long arg_0, long arg_1)
{
    return 1;
}
