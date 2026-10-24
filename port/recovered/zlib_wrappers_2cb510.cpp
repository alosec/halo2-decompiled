// @flags /O2 /GL-
// zlib 1.1.4 (third-party compression library, linked into the game): its
// one-shot helpers, here with the caller's allocator in the stream:
//   0x2cb510  compress2: the whole source deflated into the destination
//   0x2cb5b0  uncompress: the whole source inflated into the destination
// Each returns Z_OK (0) with *destination_size set to the bytes written, or
// a zlib error (Z_BUF_ERROR, -5, when the stream did not finish).
//
// Retail passes some arguments in registers (whole-program optimization); the
// stream entry points they call (deflateInit2_ 0x2cb640, deflate 0x2cb960,
// deflateEnd 0x2cbbf0, inflateInit2_ 0x2cc8d0, inflate 0x2cc9d0, inflateEnd
// 0x2cc890) take zlib's public signatures here.

typedef void *(__stdcall *block_allocate)(void *, long, long);
typedef void (__stdcall *block_free)(void *, void *);

// zlib's z_stream (0x38 bytes)
struct z_stream_2cb510
{
	unsigned char const *next_in;
	unsigned long avail_in;
	unsigned long total_in;
	unsigned char *next_out;
	unsigned long avail_out;
	unsigned long total_out;
	char *msg;
	void *state;
	block_allocate zalloc;
	block_free zfree;
	void *opaque;
	long data_type;
	unsigned long adler;
	unsigned long reserved;
};

typedef char assert_z_stream_2cb510[(sizeof(z_stream_2cb510) == 0x38) ? 1 : -1];

// the stream entry points, over their own (layout-identical) z_stream types
struct z_stream_2cb640;
struct z_stream_2ce8e0;
long __cdecl deflate_init2_2cb640(z_stream_2cb640 *strm, long level, long method, long window_bits, long mem_level,
	long strategy, char const *version, long stream_size);
long __cdecl deflate_2cb960(z_stream_2cb640 *strm, long flush);
long __cdecl deflate_end_2cbbf0(z_stream_2cb640 *strm);
long __cdecl inflate_init2_2cc8d0(z_stream_2ce8e0 *z, long w, char const *version, long stream_size);
long __cdecl inflate_2cc9d0(z_stream_2ce8e0 *z, long f);
long __cdecl inflate_end_2cc890(z_stream_2ce8e0 *z);
#define DEFLATE_STREAM_2CB510(stream) ((z_stream_2cb640 *)(stream))
#define INFLATE_STREAM_2CB510(stream) ((z_stream_2ce8e0 *)(stream))

enum
{
	Z_OK_2CB510 = 0,
	Z_STREAM_END_2CB510 = 1,
	Z_BUF_ERROR_2CB510 = -5,
	Z_FINISH_2CB510 = 4,
	Z_DEFLATED_2CB510 = 8,
	MAX_WBITS_2CB510 = 15,
	DEF_MEM_LEVEL_2CB510 = 8,
	Z_DEFAULT_STRATEGY_2CB510 = 0
};

// the version string at 0x45d934
static char const g_zlib_version_2cb510[] = "1.1.4";

// @retail 0x2cb510
long __stdcall function_2cb510(unsigned char *destination, long *destination_size, unsigned char const *source, long source_size, long level, block_allocate allocate, block_free release, void *opaque)
{
	z_stream_2cb510 stream;
	stream.next_in = source;
	stream.avail_in = (unsigned long)source_size;
	stream.next_out = destination;
	stream.avail_out = (unsigned long)*destination_size;
	stream.zalloc = allocate;
	stream.zfree = release;
	stream.opaque = opaque;
	long error = deflate_init2_2cb640(DEFLATE_STREAM_2CB510(&stream), level, Z_DEFLATED_2CB510, MAX_WBITS_2CB510, DEF_MEM_LEVEL_2CB510,
		Z_DEFAULT_STRATEGY_2CB510, g_zlib_version_2cb510, sizeof(z_stream_2cb510));
	if (error != Z_OK_2CB510)
	{
		return error;
	}
	error = deflate_2cb960(DEFLATE_STREAM_2CB510(&stream), Z_FINISH_2CB510);
	if (error != Z_STREAM_END_2CB510)
	{
		deflate_end_2cbbf0(DEFLATE_STREAM_2CB510(&stream));
		return error == Z_OK_2CB510 ? Z_BUF_ERROR_2CB510 : error;
	}
	*destination_size = (long)stream.total_out;
	return deflate_end_2cbbf0(DEFLATE_STREAM_2CB510(&stream));
}

// @retail 0x2cb5b0
long __stdcall function_2cb5b0(unsigned char *destination, long *destination_size, unsigned char const *source, long source_size, block_allocate allocate, block_free release, void *opaque)
{
	z_stream_2cb510 stream;
	stream.next_in = source;
	stream.avail_in = (unsigned long)source_size;
	stream.next_out = destination;
	stream.avail_out = (unsigned long)*destination_size;
	stream.zalloc = allocate;
	stream.zfree = release;
	stream.opaque = opaque;
	long error = inflate_init2_2cc8d0(INFLATE_STREAM_2CB510(&stream), MAX_WBITS_2CB510, g_zlib_version_2cb510, sizeof(z_stream_2cb510));
	if (error != Z_OK_2CB510)
	{
		return error;
	}
	error = inflate_2cc9d0(INFLATE_STREAM_2CB510(&stream), Z_FINISH_2CB510);
	if (error != Z_STREAM_END_2CB510)
	{
		inflate_end_2cc890(INFLATE_STREAM_2CB510(&stream));
		return error == Z_OK_2CB510 ? Z_BUF_ERROR_2CB510 : error;
	}
	*destination_size = (long)stream.total_out;
	return inflate_end_2cc890(INFLATE_STREAM_2CB510(&stream));
}
