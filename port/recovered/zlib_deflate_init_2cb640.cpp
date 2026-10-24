// @flags /O2 /GL-
// zlib 1.1.4 (third-party compression library, linked into the game): the
// start of a deflate stream (deflate.c):
//   0x2cb640  deflateInit2_: checks the version and the parameters, allocates
//             the state (0x16b8 bytes) and its window, hash chains and
//             pending buffer through the stream's allocator, then resets it
//   0x2cb850  deflateReset: the stream and its state back to the start
//
// Retail passes some arguments in registers (whole-program optimization:
// deflateInit2_ takes the stream in edx, the window bits in ecx and the
// version in eax; deflateReset the stream in eax); here they take zlib's
// public signatures, as zlib_wrappers_2cb510.cpp calls them. What they call
// and nobody has recovered (_tr_init 0x2ccdc0, lm_init 0x2cbd00, both given
// the state in edx; deflateEnd 0x2cbbf0, given the stream in esi; the
// default allocator zcalloc 0x2ce780 and zcfree 0x2ce7a0) is reached by
// retail address with zlib's signatures.

typedef void *(__stdcall *alloc_func_2cb640)(void *opaque, unsigned int items, unsigned int size);
typedef void (__stdcall *free_func_2cb640)(void *opaque, void *address);

// zlib's z_stream (0x38 bytes)
struct z_stream_2cb640
{
	unsigned char const *next_in;
	unsigned int avail_in;
	unsigned long total_in;
	unsigned char *next_out;
	unsigned int avail_out;
	unsigned long total_out;
	char *msg;
	struct deflate_state_2cb640 *state;
	alloc_func_2cb640 zalloc;
	free_func_2cb640 zfree;
	void *opaque;
	int data_type;
	unsigned long adler;
	unsigned long reserved;
};

typedef char assert_z_stream_2cb640[(sizeof(z_stream_2cb640) == 0x38) ? 1 : -1];

struct ct_data_2cb640
{
	unsigned short freq_or_code;
	unsigned short dad_or_len;
};

struct tree_desc_2cb640
{
	ct_data_2cb640 *dyn_tree;
	int max_code;
	void const *stat_desc;
};

// zlib's internal_state for deflate (0x16b8 bytes)
struct deflate_state_2cb640
{
	z_stream_2cb640 *strm;
	int status;
	unsigned char *pending_buf;
	unsigned long pending_buf_size;
	unsigned char *pending_out;
	int pending;
	int noheader;
	unsigned char data_type;
	unsigned char method;
	int last_flush;
	unsigned int w_size;
	unsigned int w_bits;
	unsigned int w_mask;
	unsigned char *window;
	unsigned long window_size;
	unsigned short *prev;
	unsigned short *head;
	unsigned int ins_h;
	unsigned int hash_size;
	unsigned int hash_bits;
	unsigned int hash_mask;
	unsigned int hash_shift;
	long block_start;
	unsigned int match_length;
	unsigned int prev_match;
	int match_available;
	unsigned int strstart;
	unsigned int match_start;
	unsigned int lookahead;
	unsigned int prev_length;
	unsigned int max_chain_length;
	unsigned int max_lazy_match;
	int level;
	int strategy;
	unsigned int good_match;
	int nice_match;
	ct_data_2cb640 dyn_ltree[573];
	ct_data_2cb640 dyn_dtree[61];
	ct_data_2cb640 bl_tree[39];
	tree_desc_2cb640 l_desc;
	tree_desc_2cb640 d_desc;
	tree_desc_2cb640 bl_desc;
	unsigned short bl_count[16];
	int heap[573];
	int heap_len;
	int heap_max;
	unsigned char depth[573];
	unsigned char *l_buf;
	unsigned int lit_bufsize;
	unsigned int last_lit;
	unsigned short *d_buf;
	unsigned long opt_len;
	unsigned long static_len;
	unsigned int matches;
	int last_eob_len;
	unsigned short bi_buf;
	int bi_valid;
};

typedef char assert_deflate_state_2cb640_level[(__builtin_offsetof(deflate_state_2cb640, level) == 0x7c) ? 1 : -1];
typedef char assert_deflate_state_2cb640_l_desc[(__builtin_offsetof(deflate_state_2cb640, l_desc) == 0xb10) ? 1 : -1];
typedef char assert_deflate_state_2cb640_l_buf[(__builtin_offsetof(deflate_state_2cb640, l_buf) == 0x1690) ? 1 : -1];
typedef char assert_deflate_state_2cb640_d_buf[(__builtin_offsetof(deflate_state_2cb640, d_buf) == 0x169c) ? 1 : -1];
typedef char assert_deflate_state_2cb640[(sizeof(deflate_state_2cb640) == 0x16b8) ? 1 : -1];

extern "C" void __stdcall h2_retail_2ccdc0(deflate_state_2cb640 *s);
extern "C" void __stdcall h2_retail_2cbd00(deflate_state_2cb640 *s);
extern "C" long h2_retail_2cbbf0(z_stream_2cb640 *stream);
extern "C" void *__stdcall h2_retail_2ce780(void *opaque, unsigned int items, unsigned int size);
extern "C" void __stdcall h2_retail_2ce7a0(void *opaque, void *address);

enum
{
	Z_OK_2CB640 = 0,
	Z_STREAM_ERROR_2CB640 = -2,
	Z_MEM_ERROR_2CB640 = -4,
	Z_VERSION_ERROR_2CB640 = -6,
	Z_DEFAULT_COMPRESSION_2CB640 = -1,
	Z_DEFLATED_2CB640 = 8,
	Z_HUFFMAN_ONLY_2CB640 = 2,
	Z_UNKNOWN_2CB640 = 2,
	Z_NO_FLUSH_2CB640 = 0,
	MAX_MEM_LEVEL_2CB640 = 9,
	MIN_MATCH_2CB640 = 3,
	INIT_STATE_2CB640 = 42,
	BUSY_STATE_2CB640 = 113
};

// the version the library was built as (the string at 0x45d934)
static char const g_zlib_version_2cb640[] = "1.1.4";

// @retail 0x2cb850
long __cdecl deflate_reset_2cb850(z_stream_2cb640 *strm)
{
	if (strm == 0 || strm->state == 0 || strm->zalloc == 0 || strm->zfree == 0)
	{
		return Z_STREAM_ERROR_2CB640;
	}
	strm->total_in = strm->total_out = 0;
	strm->msg = 0;
	strm->data_type = Z_UNKNOWN_2CB640;

	deflate_state_2cb640 *s = strm->state;
	s->pending = 0;
	s->pending_out = s->pending_buf;
	if (s->noheader < 0)
	{
		// set to -1 by deflate(..., Z_FINISH)
		s->noheader = 0;
	}
	s->status = s->noheader ? BUSY_STATE_2CB640 : INIT_STATE_2CB640;
	strm->adler = 1;
	s->last_flush = Z_NO_FLUSH_2CB640;

	h2_retail_2ccdc0(s);
	h2_retail_2cbd00(s);
	return Z_OK_2CB640;
}

// @retail 0x2cb640
long __cdecl deflate_init2_2cb640(z_stream_2cb640 *strm, long level, long method, long window_bits, long mem_level,
	long strategy, char const *version, long stream_size)
{
	int noheader = 0;
	if (version == 0 || version[0] != g_zlib_version_2cb640[0] || stream_size != (long)sizeof(z_stream_2cb640))
	{
		return Z_VERSION_ERROR_2CB640;
	}
	if (strm == 0)
	{
		return Z_STREAM_ERROR_2CB640;
	}

	strm->msg = 0;
	if (strm->zalloc == 0)
	{
		strm->zalloc = h2_retail_2ce780;
		strm->opaque = 0;
	}
	if (strm->zfree == 0)
	{
		strm->zfree = h2_retail_2ce7a0;
	}

	if (level == Z_DEFAULT_COMPRESSION_2CB640)
	{
		level = 6;
	}
	if (window_bits < 0)
	{
		// undocumented: no zlib header
		noheader = 1;
		window_bits = -window_bits;
	}
	if (mem_level < 1 || mem_level > MAX_MEM_LEVEL_2CB640 || method != Z_DEFLATED_2CB640 || window_bits < 9 ||
		window_bits > 15 || level < 0 || level > 9 || strategy < 0 || strategy > Z_HUFFMAN_ONLY_2CB640)
	{
		return Z_STREAM_ERROR_2CB640;
	}

	deflate_state_2cb640 *s = (deflate_state_2cb640 *)strm->zalloc(strm->opaque, 1, sizeof(deflate_state_2cb640));
	if (s == 0)
	{
		return Z_MEM_ERROR_2CB640;
	}
	strm->state = s;
	s->strm = strm;

	s->noheader = noheader;
	s->w_bits = (unsigned int)window_bits;
	s->w_size = 1u << s->w_bits;
	s->w_mask = s->w_size - 1;

	s->hash_bits = (unsigned int)mem_level + 7;
	s->hash_size = 1u << s->hash_bits;
	s->hash_mask = s->hash_size - 1;
	s->hash_shift = (s->hash_bits + MIN_MATCH_2CB640 - 1) / MIN_MATCH_2CB640;

	s->window = (unsigned char *)strm->zalloc(strm->opaque, s->w_size, 2 * sizeof(unsigned char));
	s->prev = (unsigned short *)strm->zalloc(strm->opaque, s->w_size, sizeof(unsigned short));
	s->head = (unsigned short *)strm->zalloc(strm->opaque, s->hash_size, sizeof(unsigned short));

	// 16K elements by default
	s->lit_bufsize = 1u << (mem_level + 6);

	unsigned short *overlay = (unsigned short *)strm->zalloc(strm->opaque, s->lit_bufsize, sizeof(unsigned short) + 2);
	s->pending_buf = (unsigned char *)overlay;
	s->pending_buf_size = (unsigned long)s->lit_bufsize * (sizeof(unsigned short) + 2L);

	if (s->window == 0 || s->prev == 0 || s->head == 0 || s->pending_buf == 0)
	{
		// z_errmsg's entry for Z_MEM_ERROR
		strm->msg = (char *)"insufficient memory";
		h2_retail_2cbbf0(strm);
		return Z_MEM_ERROR_2CB640;
	}
	s->d_buf = overlay + s->lit_bufsize / sizeof(unsigned short);
	s->l_buf = s->pending_buf + (1 + sizeof(unsigned short)) * s->lit_bufsize;

	s->level = (int)level;
	s->strategy = (int)strategy;
	s->method = (unsigned char)method;

	return deflate_reset_2cb850(strm);
}
