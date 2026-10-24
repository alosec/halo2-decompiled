// zlib 1.1.4's deflate state (deflate.h), shared by the units that recover
// deflate.c (zlib_deflate_2cb8c0.cpp) and trees.c (zlib_trees_2ccdc0.cpp).
// The types keep the names (and exact layout) zlib_deflate_init_2cb640.cpp
// gives them, so a function declared over them here is the same function
// there.
#ifndef ZLIB_DEFLATE_2CB8C0_H
#define ZLIB_DEFLATE_2CB8C0_H

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

// a Huffman tree node: freq/code, dad/len
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

typedef char assert_z_stream_2cb8c0[(sizeof(z_stream_2cb640) == 0x38) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_lookahead[(__builtin_offsetof(deflate_state_2cb640, lookahead) == 0x6c) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_level[(__builtin_offsetof(deflate_state_2cb640, level) == 0x7c) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_dyn_ltree[(__builtin_offsetof(deflate_state_2cb640, dyn_ltree) == 0x8c) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_dyn_dtree[(__builtin_offsetof(deflate_state_2cb640, dyn_dtree) == 0x980) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_bl_tree[(__builtin_offsetof(deflate_state_2cb640, bl_tree) == 0xa74) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_l_desc[(__builtin_offsetof(deflate_state_2cb640, l_desc) == 0xb10) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_l_buf[(__builtin_offsetof(deflate_state_2cb640, l_buf) == 0x1690) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_last_lit[(__builtin_offsetof(deflate_state_2cb640, last_lit) == 0x1698) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0_bi_buf[(__builtin_offsetof(deflate_state_2cb640, bi_buf) == 0x16b0) ? 1 : -1];
typedef char assert_deflate_state_2cb8c0[(sizeof(deflate_state_2cb640) == 0x16b8) ? 1 : -1];

// deflate.h's constants
enum
{
	LENGTH_CODES_2CB8C0 = 29,
	LITERALS_2CB8C0 = 256,
	L_CODES_2CB8C0 = LITERALS_2CB8C0 + 1 + LENGTH_CODES_2CB8C0,
	D_CODES_2CB8C0 = 30,
	BL_CODES_2CB8C0 = 19,
	HEAP_SIZE_2CB8C0 = 2 * L_CODES_2CB8C0 + 1,
	MAX_BITS_2CB8C0 = 15,
	MIN_MATCH_2CB8C0 = 3,
	MAX_MATCH_2CB8C0 = 258,
	MIN_LOOKAHEAD_2CB8C0 = MAX_MATCH_2CB8C0 + MIN_MATCH_2CB8C0 + 1
};

// trees.c (zlib_trees_2ccdc0.cpp)
extern unsigned char const g_dist_code_2ccdc0[512];   // _dist_code (retail 0x450038)
extern unsigned char const g_length_code_2ccdc0[256]; // _length_code (retail 0x450238)
void __cdecl tr_init_2ccdc0(deflate_state_2cb640 *s);
void __cdecl tr_stored_block_2cdce0(deflate_state_2cb640 *s, char *buf, unsigned long stored_len, int eof);
void __cdecl tr_align_2cdd80(deflate_state_2cb640 *s);
void __cdecl tr_flush_block_2cdf70(deflate_state_2cb640 *s, char *buf, unsigned long stored_len, int eof);

// deflate.c (zlib_deflate_2cb8c0.cpp)
void __cdecl lm_init_2cbd00(deflate_state_2cb640 *s);
long __cdecl deflate_2cb960(z_stream_2cb640 *strm, long flush);
long __cdecl deflate_end_2cbbf0(z_stream_2cb640 *strm);

// zutil.c / adler32.c (zlib_util_2ce780.cpp)
void *__stdcall zcalloc_2ce780(void *opaque, unsigned int items, unsigned int size);
void __stdcall zcfree_2ce7a0(void *opaque, void *address);
unsigned long __stdcall adler32_2ce7b0(unsigned long adler, unsigned char const *buf, unsigned int len);

#endif
