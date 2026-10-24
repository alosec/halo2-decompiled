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
// 0x2cc890) are not recovered and are called by retail address with zlib's
// public signatures.

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

extern "C" long h2_retail_2cb640(z_stream_2cb510 *stream, long level, long method, long window_bits, long memory_level,
	long strategy, char const *version, long stream_size);
extern "C" long h2_retail_2cb960(z_stream_2cb510 *stream, long flush);
extern "C" long h2_retail_2cbbf0(z_stream_2cb510 *stream);
extern "C" long h2_retail_2cc8d0(z_stream_2cb510 *stream, long window_bits, char const *version, long stream_size);
extern "C" long h2_retail_2cc9d0(z_stream_2cb510 *stream, long flush);
extern "C" long h2_retail_2cc890(z_stream_2cb510 *stream);

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
	long error = h2_retail_2cb640(&stream, level, Z_DEFLATED_2CB510, MAX_WBITS_2CB510, DEF_MEM_LEVEL_2CB510,
		Z_DEFAULT_STRATEGY_2CB510, g_zlib_version_2cb510, sizeof(z_stream_2cb510));
	if (error != Z_OK_2CB510)
	{
		return error;
	}
	error = h2_retail_2cb960(&stream, Z_FINISH_2CB510);
	if (error != Z_STREAM_END_2CB510)
	{
		h2_retail_2cbbf0(&stream);
		return error == Z_OK_2CB510 ? Z_BUF_ERROR_2CB510 : error;
	}
	*destination_size = (long)stream.total_out;
	return h2_retail_2cbbf0(&stream);
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
	long error = h2_retail_2cc8d0(&stream, MAX_WBITS_2CB510, g_zlib_version_2cb510, sizeof(z_stream_2cb510));
	if (error != Z_OK_2CB510)
	{
		return error;
	}
	error = h2_retail_2cc9d0(&stream, Z_FINISH_2CB510);
	if (error != Z_STREAM_END_2CB510)
	{
		h2_retail_2cc890(&stream);
		return error == Z_OK_2CB510 ? Z_BUF_ERROR_2CB510 : error;
	}
	*destination_size = (long)stream.total_out;
	return h2_retail_2cc890(&stream);
}
