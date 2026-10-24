// @flags /O2 /GL-
// zlib 1.1.4 (third-party compression library, linked into the game):
// inflate.c, the zlib-format layer over inflate_blocks
// (zlib_inflate_blocks_2ce8e0.cpp):
//   0x2cc850  inflateReset
//   0x2cc890  inflateEnd
//   0x2cc8d0  inflateInit2_
//   0x2cc9d0  inflate
// Stock 1.1.4. inflateInit_, inflateSetDictionary, inflateSync and
// inflateSyncPoint are not in the image.
//
// Whole-program optimization moved arguments into registers; here every
// function takes zlib's own parameters in zlib's order. Retail's registers:
//   inflateReset   z eax
//   inflateEnd     z eax
//   inflateInit2_  z esi, w ecx, version eax (stream_size on the stack)
//   inflate        z eax, f edx
// The error strings live in retail at 0x45d93c..0x45d990; here they are
// literals.
typedef void *(__stdcall *alloc_func_2ce8e0)(void *opaque, unsigned int items, unsigned int size);
typedef void (__stdcall *free_func_2ce8e0)(void *opaque, void *address);
typedef unsigned long (__stdcall *check_func_2ce8e0)(unsigned long check, unsigned char const *buffer,
	unsigned int length);

// zlib's z_stream (0x38 bytes), as zlib_inflate_blocks_2ce8e0.cpp has it
struct z_stream_2ce8e0
{
	unsigned char *next_in;
	unsigned int avail_in;
	unsigned long total_in;
	unsigned char *next_out;
	unsigned int avail_out;
	unsigned long total_out;
	char *msg;
	void *state;
	alloc_func_2ce8e0 zalloc;
	free_func_2ce8e0 zfree;
	void *opaque;
	int data_type;
	unsigned long adler;
	unsigned long reserved;
};

typedef char assert_z_stream_2cc850[(sizeof(z_stream_2ce8e0) == 0x38) ? 1 : -1];

// zutil.c / adler32.c (zlib_util_2ce780.cpp)
void *__stdcall zcalloc_2ce780(void *opaque, unsigned int items, unsigned int size);
void __stdcall zcfree_2ce7a0(void *opaque, void *address);
unsigned long __stdcall adler32_2ce7b0(unsigned long adler, unsigned char const *buf, unsigned int len);

// infblock.c (zlib_inflate_blocks_2ce8e0.cpp)
struct inflate_blocks_state_2ce8e0;
void __cdecl inflate_blocks_reset_2ce8e0(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z, unsigned long *c);
inflate_blocks_state_2ce8e0 *__cdecl inflate_blocks_new_2ce950(z_stream_2ce8e0 *z, check_func_2ce8e0 c,
	unsigned int w);
int __cdecl inflate_blocks_2ce9e0(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z, int r);
int __cdecl inflate_blocks_free_2cf480(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z);

enum
{
	Z_OK_2CC850 = 0,
	Z_STREAM_END_2CC850 = 1,
	Z_NEED_DICT_2CC850 = 2,
	Z_STREAM_ERROR_2CC850 = -2,
	Z_DATA_ERROR_2CC850 = -3,
	Z_MEM_ERROR_2CC850 = -4,
	Z_BUF_ERROR_2CC850 = -5,
	Z_VERSION_ERROR_2CC850 = -6,
	Z_FINISH_2CC850 = 4,
	Z_DEFLATED_2CC850 = 8,
	PRESET_DICT_2CC850 = 0x20
};

// inflate's modes
enum inflate_mode_2cc850
{
	METHOD_2CC850, // waiting for method byte
	FLAG_2CC850,   // waiting for flag byte
	DICT4_2CC850,  // four dictionary check bytes to go
	DICT3_2CC850,  // three dictionary check bytes to go
	DICT2_2CC850,  // two dictionary check bytes to go
	DICT1_2CC850,  // one dictionary check byte to go
	DICT0_2CC850,  // waiting for inflateSetDictionary
	BLOCKS_2CC850, // decompressing blocks
	CHECK4_2CC850, // four check bytes to go
	CHECK3_2CC850, // three check bytes to go
	CHECK2_2CC850, // two check bytes to go
	CHECK1_2CC850, // one check byte to go
	DONE_2CC850,   // finished check, done
	BAD_2CC850     // got an error--stay here
};

// inflate's internal_state (0x18 bytes)
struct inflate_state_2cc850
{
	inflate_mode_2cc850 mode; // current inflate mode
	union
	{
		unsigned int method; // if FLAGS, method byte
		struct
		{
			unsigned long was;  // computed check value
			unsigned long need; // stream check value
		} check;             // if CHECK, check values to compare
		unsigned int marker; // if BAD, inflateSync's marker bytes count
	} sub;
	int nowrap;                          // flag for no wrapper
	unsigned int wbits;                  // log2(window size) (8..15, defaults to 15)
	inflate_blocks_state_2ce8e0 *blocks; // current inflate_blocks state
};

typedef char assert_inflate_state_2cc850[(sizeof(inflate_state_2cc850) == 0x18) ? 1 : -1];

#define STATE_2CC850(z) ((inflate_state_2cc850 *)(z)->state)

// the version the library was built as (the string at 0x45d934)
static char const g_zlib_version_2cc850[] = "1.1.4";

// @retail 0x2cc850
long __cdecl inflate_reset_2cc850(z_stream_2ce8e0 *z)
{
	if (z == 0 || z->state == 0)
	{
		return Z_STREAM_ERROR_2CC850;
	}
	z->total_in = z->total_out = 0;
	z->msg = 0;
	STATE_2CC850(z)->mode = STATE_2CC850(z)->nowrap ? BLOCKS_2CC850 : METHOD_2CC850;
	inflate_blocks_reset_2ce8e0(STATE_2CC850(z)->blocks, z, 0);
	return Z_OK_2CC850;
}

// @retail 0x2cc890
long __cdecl inflate_end_2cc890(z_stream_2ce8e0 *z)
{
	if (z == 0 || z->state == 0 || z->zfree == 0)
	{
		return Z_STREAM_ERROR_2CC850;
	}
	if (STATE_2CC850(z)->blocks != 0)
	{
		inflate_blocks_free_2cf480(STATE_2CC850(z)->blocks, z);
	}
	z->zfree(z->opaque, z->state);
	z->state = 0;
	return Z_OK_2CC850;
}

// @retail 0x2cc8d0
long __cdecl inflate_init2_2cc8d0(z_stream_2ce8e0 *z, long w, char const *version, long stream_size)
{
	if (version == 0 || version[0] != g_zlib_version_2cc850[0] || stream_size != (long)sizeof(z_stream_2ce8e0))
	{
		return Z_VERSION_ERROR_2CC850;
	}

	// initialize state
	if (z == 0)
	{
		return Z_STREAM_ERROR_2CC850;
	}
	z->msg = 0;
	if (z->zalloc == 0)
	{
		z->zalloc = zcalloc_2ce780;
		z->opaque = 0;
	}
	if (z->zfree == 0)
	{
		z->zfree = zcfree_2ce7a0;
	}
	if ((z->state = z->zalloc(z->opaque, 1, sizeof(inflate_state_2cc850))) == 0)
	{
		return Z_MEM_ERROR_2CC850;
	}
	STATE_2CC850(z)->blocks = 0;

	// handle undocumented nowrap option (no zlib header or check)
	STATE_2CC850(z)->nowrap = 0;
	if (w < 0)
	{
		w = -w;
		STATE_2CC850(z)->nowrap = 1;
	}

	// set window size
	if (w < 8 || w > 15)
	{
		inflate_end_2cc890(z);
		return Z_STREAM_ERROR_2CC850;
	}
	STATE_2CC850(z)->wbits = (unsigned int)w;

	// create inflate_blocks state
	if ((STATE_2CC850(z)->blocks = inflate_blocks_new_2ce950(z, STATE_2CC850(z)->nowrap ? 0 : adler32_2ce7b0,
			 (unsigned int)1 << w)) == 0)
	{
		inflate_end_2cc890(z);
		return Z_MEM_ERROR_2CC850;
	}

	// reset state
	inflate_reset_2cc850(z);
	return Z_OK_2CC850;
}

#define NEEDBYTE_2CC850 {if (z->avail_in == 0) return r; r = f;}
#define NEXTBYTE_2CC850 (z->avail_in--, z->total_in++, *z->next_in++)

// @retail 0x2cc9d0
long __cdecl inflate_2cc9d0(z_stream_2ce8e0 *z, long f)
{
	int r;
	unsigned int b;

	if (z == 0 || z->state == 0 || z->next_in == 0)
	{
		return Z_STREAM_ERROR_2CC850;
	}
	f = f == Z_FINISH_2CC850 ? Z_BUF_ERROR_2CC850 : Z_OK_2CC850;
	r = Z_BUF_ERROR_2CC850;
	while (1)
	{
		switch (STATE_2CC850(z)->mode)
		{
		case METHOD_2CC850:
			NEEDBYTE_2CC850
			if (((STATE_2CC850(z)->sub.method = NEXTBYTE_2CC850) & 0xf) != Z_DEFLATED_2CC850)
			{
				STATE_2CC850(z)->mode = BAD_2CC850;
				z->msg = (char *)"unknown compression method";
				STATE_2CC850(z)->sub.marker = 5; // can't try inflateSync
				break;
			}
			if ((STATE_2CC850(z)->sub.method >> 4) + 8 > STATE_2CC850(z)->wbits)
			{
				STATE_2CC850(z)->mode = BAD_2CC850;
				z->msg = (char *)"invalid window size";
				STATE_2CC850(z)->sub.marker = 5; // can't try inflateSync
				break;
			}
			STATE_2CC850(z)->mode = FLAG_2CC850;
			// fall through
		case FLAG_2CC850:
			NEEDBYTE_2CC850
			b = NEXTBYTE_2CC850;
			if (((STATE_2CC850(z)->sub.method << 8) + b) % 31)
			{
				STATE_2CC850(z)->mode = BAD_2CC850;
				z->msg = (char *)"incorrect header check";
				STATE_2CC850(z)->sub.marker = 5; // can't try inflateSync
				break;
			}
			if (!(b & PRESET_DICT_2CC850))
			{
				STATE_2CC850(z)->mode = BLOCKS_2CC850;
				break;
			}
			STATE_2CC850(z)->mode = DICT4_2CC850;
			// fall through
		case DICT4_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need = (unsigned long)NEXTBYTE_2CC850 << 24;
			STATE_2CC850(z)->mode = DICT3_2CC850;
			// fall through
		case DICT3_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need += (unsigned long)NEXTBYTE_2CC850 << 16;
			STATE_2CC850(z)->mode = DICT2_2CC850;
			// fall through
		case DICT2_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need += (unsigned long)NEXTBYTE_2CC850 << 8;
			STATE_2CC850(z)->mode = DICT1_2CC850;
			// fall through
		case DICT1_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need += (unsigned long)NEXTBYTE_2CC850;
			z->adler = STATE_2CC850(z)->sub.check.need;
			STATE_2CC850(z)->mode = DICT0_2CC850;
			return Z_NEED_DICT_2CC850;
		case DICT0_2CC850:
			STATE_2CC850(z)->mode = BAD_2CC850;
			z->msg = (char *)"need dictionary";
			STATE_2CC850(z)->sub.marker = 0; // can try inflateSync
			return Z_STREAM_ERROR_2CC850;
		case BLOCKS_2CC850:
			r = inflate_blocks_2ce9e0(STATE_2CC850(z)->blocks, z, r);
			if (r == Z_DATA_ERROR_2CC850)
			{
				STATE_2CC850(z)->mode = BAD_2CC850;
				STATE_2CC850(z)->sub.marker = 0; // can try inflateSync
				break;
			}
			if (r == Z_OK_2CC850)
			{
				r = f;
			}
			if (r != Z_STREAM_END_2CC850)
			{
				return r;
			}
			r = f;
			inflate_blocks_reset_2ce8e0(STATE_2CC850(z)->blocks, z, &STATE_2CC850(z)->sub.check.was);
			if (STATE_2CC850(z)->nowrap)
			{
				STATE_2CC850(z)->mode = DONE_2CC850;
				break;
			}
			STATE_2CC850(z)->mode = CHECK4_2CC850;
			// fall through
		case CHECK4_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need = (unsigned long)NEXTBYTE_2CC850 << 24;
			STATE_2CC850(z)->mode = CHECK3_2CC850;
			// fall through
		case CHECK3_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need += (unsigned long)NEXTBYTE_2CC850 << 16;
			STATE_2CC850(z)->mode = CHECK2_2CC850;
			// fall through
		case CHECK2_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need += (unsigned long)NEXTBYTE_2CC850 << 8;
			STATE_2CC850(z)->mode = CHECK1_2CC850;
			// fall through
		case CHECK1_2CC850:
			NEEDBYTE_2CC850
			STATE_2CC850(z)->sub.check.need += (unsigned long)NEXTBYTE_2CC850;

			if (STATE_2CC850(z)->sub.check.was != STATE_2CC850(z)->sub.check.need)
			{
				STATE_2CC850(z)->mode = BAD_2CC850;
				z->msg = (char *)"incorrect data check";
				STATE_2CC850(z)->sub.marker = 5; // can't try inflateSync
				break;
			}
			STATE_2CC850(z)->mode = DONE_2CC850;
			// fall through
		case DONE_2CC850:
			return Z_STREAM_END_2CC850;
		case BAD_2CC850:
			return Z_DATA_ERROR_2CC850;
		default:
			return Z_STREAM_ERROR_2CC850;
		}
	}
}
