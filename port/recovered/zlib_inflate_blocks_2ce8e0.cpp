// @flags /O2 /GL-
// zlib 1.1.4 (third-party compression library, linked into the game): the
// inflate internals below inflate() (0x2cc9d0) and its setup:
//   infblock.c  0x2ce8e0 inflate_blocks_reset, 0x2ce950 inflate_blocks_new,
//               0x2ce9e0 inflate_blocks, 0x2cf480 inflate_blocks_free
//               (inflate_trees_fixed and inflate_codes_free are inlined)
//   inftrees.c  0x2cf4f0 huft_build, 0x2cf9e0 inflate_trees_bits,
//               0x2cfa80 inflate_trees_dynamic
//   infutil.c   0x2cfbf0 inflate_flush
//   infcodes.c  0x2cfd30 inflate_codes_new, 0x2cfd70 inflate_codes
//   inffast.c   0x2d0460 inflate_fast
// The tables are the library's (the copies retail keeps: border 0x450428,
// cplens 0x4504a8, cplext 0x450528, cpdist 0x4505a8, cpdext 0x450620,
// inflate_mask 0x476ef0); the fixed trees (inffixed.h: fixed_bl 0x475de8,
// fixed_bd 0x475dec, fixed_tl 0x475df0, fixed_td 0x476df0) are read where
// retail keeps them.
//
// Whole-program optimization moved some arguments into registers; here every
// function takes zlib's own parameters in zlib's order. Retail's registers:
//   inflate_blocks_reset  s esi, z edi, c eax
//   inflate_blocks_new    z eax, w ebx (c on the stack)
//   inflate_blocks_free   s esi, z edi
//   huft_build            b ecx, m eax (the rest on the stack)
//   inflate_trees_bits, inflate_trees_dynamic  z ebx
//   inflate_flush         s ebx
//   inflate_codes_new     z eax
//   inflate_codes         s eax, z esi
// (inflate_blocks and inflate_fast take everything on the stack.)
#include <string.h>

typedef void *(__stdcall *alloc_func_2ce8e0)(void *opaque, unsigned int items, unsigned int size);
typedef void (__stdcall *free_func_2ce8e0)(void *opaque, void *address);
typedef unsigned long (__stdcall *check_func_2ce8e0)(unsigned long check, unsigned char const *buffer,
	unsigned int length);

// zlib's z_stream (0x38 bytes)
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

typedef char assert_z_stream_2ce8e0[(sizeof(z_stream_2ce8e0) == 0x38) ? 1 : -1];

#define ZALLOC_2CE8E0(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE_2CE8E0(strm, address) (*((strm)->zfree))((strm)->opaque, (void *)(address))

enum
{
	Z_OK_2CE8E0 = 0,
	Z_STREAM_END_2CE8E0 = 1,
	Z_STREAM_ERROR_2CE8E0 = -2,
	Z_DATA_ERROR_2CE8E0 = -3,
	Z_MEM_ERROR_2CE8E0 = -4,
	Z_BUF_ERROR_2CE8E0 = -5,
	// maximum bit length of any code, and the most hufts a stream's trees use
	BMAX_2CE8E0 = 15,
	MANY_2CE8E0 = 1440
};

// one entry of a decoding table
struct inflate_huft_2ce8e0
{
	unsigned char exop;  // number of extra bits or operation
	unsigned char bits;  // number of bits in this code or subcode
	unsigned short pad;
	unsigned int base;   // literal, length base, distance base, or table offset
};

typedef char assert_inflate_huft_2ce8e0[(sizeof(inflate_huft_2ce8e0) == 8) ? 1 : -1];

// infcodes.c's state (0x1c bytes)
enum
{
	START_2CE8E0,   // x: set up for LEN
	LEN_2CE8E0,     // i: get length/literal/eob next
	LENEXT_2CE8E0,  // i: getting length extra (have base)
	DIST_2CE8E0,    // i: get distance next
	DISTEXT_2CE8E0, // i: getting distance extra
	COPY_2CE8E0,    // o: copying bytes in window, waiting for space
	LIT_2CE8E0,     // o: got literal, waiting for output space
	WASH_2CE8E0,    // o: got eob, possibly still output waiting
	END_2CE8E0,     // x: got eob and all data flushed
	BADCODE_2CE8E0  // x: got error
};

struct inflate_codes_state_2ce8e0
{
	int mode;
	unsigned int len;
	union
	{
		struct
		{
			inflate_huft_2ce8e0 *tree; // pointer into tree
			unsigned int need;         // bits needed
		} code;
		unsigned int lit;
		struct
		{
			unsigned int get;  // bits to get for extra
			unsigned int dist; // distance back to copy from
		} copy;
	} sub;
	unsigned char lbits;
	unsigned char dbits;
	inflate_huft_2ce8e0 *ltree;
	inflate_huft_2ce8e0 *dtree;
};

typedef char assert_inflate_codes_state_2ce8e0[(sizeof(inflate_codes_state_2ce8e0) == 0x1c) ? 1 : -1];

// infblock.c's state (0x40 bytes)
enum
{
	TYPE_2CE8E0,   // get type bits (3, including end bit)
	LENS_2CE8E0,   // get lengths for stored
	STORED_2CE8E0, // processing stored block
	TABLE_2CE8E0,  // get table lengths
	BTREE_2CE8E0,  // get bit lengths tree for a dynamic block
	DTREE_2CE8E0,  // get length, distance trees for a dynamic block
	CODES_2CE8E0,  // processing fixed or dynamic block
	DRY_2CE8E0,    // output remaining window bytes
	DONE_2CE8E0,   // finished last block, done
	BAD_2CE8E0     // got a data error--stuck here
};

struct inflate_blocks_state_2ce8e0
{
	int mode;
	union
	{
		unsigned int left; // if STORED, bytes left to copy
		struct
		{
			unsigned int table;       // table lengths (14 bits)
			unsigned int index;       // index into blens (or border)
			unsigned int *blens;      // bit lengths of codes
			unsigned int bb;          // bit length tree depth
			inflate_huft_2ce8e0 *tb;  // bit length decoding tree
		} trees;
		struct
		{
			inflate_codes_state_2ce8e0 *codes;
		} decode;
	} sub;
	unsigned int last;
	unsigned int bitk;
	unsigned long bitb;
	inflate_huft_2ce8e0 *hufts;
	unsigned char *window;
	unsigned char *end;
	unsigned char *read;
	unsigned char *write;
	check_func_2ce8e0 checkfn;
	unsigned long check;
};

typedef char assert_inflate_blocks_state_2ce8e0[(sizeof(inflate_blocks_state_2ce8e0) == 0x40) ? 1 : -1];

// ---------------------------------------------------------------------------
// the tables

// the order of the bit length code lengths
static unsigned int const g_border_2ce8e0[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

// copy lengths for literal codes 257..285, and their extra bits (112 marks
// the two invalid codes)
static unsigned int const g_cplens_2ce8e0[31] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
static unsigned int const g_cplext_2ce8e0[31] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112};
// copy offsets for distance codes 0..29, and their extra bits
static unsigned int const g_cpdist_2ce8e0[30] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
	8193, 12289, 16385, 24577};
static unsigned int const g_cpdext_2ce8e0[30] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

// masks for the low n bits
static unsigned int const g_inflate_mask_2ce8e0[17] = {
	0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
	0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff};

// the fixed trees (inffixed.h), where retail keeps them
extern unsigned int const g_475de8;              // fixed_bl
extern unsigned int const g_475dec;              // fixed_bd
extern inflate_huft_2ce8e0 const g_475df0[512];  // fixed_tl
extern inflate_huft_2ce8e0 const g_476df0[32];   // fixed_td

// ---------------------------------------------------------------------------
// infutil.h's macros, over the locals every routine keeps

// update pointers and return
#define UPDBITS {s->bitb = b; s->bitk = k;}
#define UPDIN {z->avail_in = n; z->total_in += (unsigned long)(p - z->next_in); z->next_in = p;}
#define UPDOUT {s->write = q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush_2cfbf0(s, z, r);}
// get bytes and bits
#define LOADIN {p = z->next_in; n = z->avail_in; b = s->bitb; k = s->bitk;}
#define NEEDBYTE {if (n) r = Z_OK_2CE8E0; else LEAVE}
#define NEXTBYTE (n--, *p++)
#define NEEDBITS(j) {while (k < (j)) {NEEDBYTE; b |= ((unsigned long)NEXTBYTE) << k; k += 8;}}
#define DUMPBITS(j) {b >>= (j); k -= (j);}
// output bytes
#define WAVAIL (unsigned int)(q < s->read ? s->read - q - 1 : s->end - q)
#define LOADOUT {q = s->write; m = (unsigned int)WAVAIL;}
#define WRAP {if (q == s->end && s->read != s->window) {q = s->window; m = (unsigned int)WAVAIL;}}
#define FLUSH {UPDOUT r = inflate_flush_2cfbf0(s, z, r); LOADOUT}
#define NEEDOUT {if (m == 0) {WRAP if (m == 0) {FLUSH WRAP if (m == 0) LEAVE}} r = Z_OK_2CE8E0;}
#define OUTBYTE(a) {*q++ = (unsigned char)(a); m--;}
// load local pointers
#define LOAD {LOADIN LOADOUT}

// ---------------------------------------------------------------------------
// infutil.c

// copies as much as possible from the sliding window to the output area
// @retail 0x2cfbf0
int __cdecl inflate_flush_2cfbf0(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z, int r)
{
	// local copies of source and destination pointers
	unsigned char *p = z->next_out;
	unsigned char *q = s->read;

	// compute number of bytes to copy as far as end of window
	unsigned int n = (unsigned int)((q <= s->write ? s->write : s->end) - q);
	if (n > z->avail_out)
	{
		n = z->avail_out;
	}
	if (n && r == Z_BUF_ERROR_2CE8E0)
	{
		r = Z_OK_2CE8E0;
	}

	// update counters
	z->avail_out -= n;
	z->total_out += n;

	// update check information
	if (s->checkfn != 0)
	{
		z->adler = s->check = (*s->checkfn)(s->check, q, n);
	}

	// copy as far as end of window
	memcpy(p, q, n);
	p += n;
	q += n;

	// see if more to copy at beginning of window
	if (q == s->end)
	{
		// wrap pointers
		q = s->window;
		if (s->write == s->end)
		{
			s->write = s->window;
		}

		// compute bytes to copy
		n = (unsigned int)(s->write - q);
		if (n > z->avail_out)
		{
			n = z->avail_out;
		}
		if (n && r == Z_BUF_ERROR_2CE8E0)
		{
			r = Z_OK_2CE8E0;
		}

		// update counters
		z->avail_out -= n;
		z->total_out += n;

		// update check information
		if (s->checkfn != 0)
		{
			z->adler = s->check = (*s->checkfn)(s->check, q, n);
		}

		// copy
		memcpy(p, q, n);
		p += n;
		q += n;
	}

	// update pointers
	z->next_out = p;
	s->read = q;

	return r;
}

// ---------------------------------------------------------------------------
// inftrees.c

// Given a list of code lengths and a maximum table size, make a set of
// tables to decode that set of codes. Return Z_OK on success, Z_BUF_ERROR if
// the given code set is incomplete (the tables are still built in this
// case), or Z_DATA_ERROR if the input is invalid (an over-subscribed set of
// lengths) or the hufts would run out.
// @retail 0x2cf4f0
int __cdecl huft_build_2cf4f0(
	unsigned int *b,                // code lengths in bits (all assumed <= BMAX)
	unsigned int n,                 // number of codes (assumed <= 288)
	unsigned int s,                 // number of simple-valued codes (0..s-1)
	unsigned int const *d,          // list of base values for non-simple codes
	unsigned int const *e,          // list of extra bits for non-simple codes
	inflate_huft_2ce8e0 **t,        // result: starting table
	unsigned int *m,                // maximum lookup bits, returns actual
	inflate_huft_2ce8e0 *hp,        // space for trees
	unsigned int *hn,               // hufts used in space
	unsigned int *v)                // working area: values in order of bit length
{
	unsigned int a;                         // counter for codes of length k
	unsigned int c[BMAX_2CE8E0 + 1];        // bit length count table
	unsigned int f;                         // i repeats in table every f entries
	int g;                                  // maximum code length
	int h;                                  // table level
	unsigned int i;                         // counter, current code
	unsigned int j;                         // counter
	int k;                                  // number of bits in current code
	int l;                                  // bits per table (returned in m)
	unsigned int mask;                      // (1 << w) - 1
	unsigned int *p;                        // pointer into c[], b[], or v[]
	inflate_huft_2ce8e0 *q;                 // points to current table
	inflate_huft_2ce8e0 r;                  // table entry for structure assignment
	inflate_huft_2ce8e0 *u[BMAX_2CE8E0];    // table stack
	int w;                                  // bits before this table == (l * h)
	unsigned int x[BMAX_2CE8E0 + 1];        // bit offsets, then code stack
	unsigned int *xp;                       // pointer into x
	int y;                                  // number of dummy codes added
	unsigned int z;                         // number of entries in current table

	// an entry for an invalid code keeps the base last set (retail leaves it
	// as it finds it on the stack until then)
	r.pad = 0;
	r.base = 0;

	// generate counts for each bit length
	for (p = c; p < c + BMAX_2CE8E0 + 1; p++)
	{
		*p = 0;
	}
	p = b;
	i = n;
	do
	{
		c[*p++]++; // assume all entries <= BMAX
	} while (--i);
	if (c[0] == n) // null input--all zero length codes
	{
		*t = 0;
		*m = 0;
		return Z_OK_2CE8E0;
	}

	// find minimum and maximum length, bound *m by those
	l = (int)*m;
	for (j = 1; j <= BMAX_2CE8E0; j++)
	{
		if (c[j])
		{
			break;
		}
	}
	k = (int)j; // minimum code length
	if ((unsigned int)l < j)
	{
		l = (int)j;
	}
	for (i = BMAX_2CE8E0; i; i--)
	{
		if (c[i])
		{
			break;
		}
	}
	g = (int)i; // maximum code length
	if ((unsigned int)l > i)
	{
		l = (int)i;
	}
	*m = (unsigned int)l;

	// adjust last length count to fill out codes, if needed
	for (y = 1 << j; j < i; j++, y <<= 1)
	{
		if ((y -= (int)c[j]) < 0)
		{
			return Z_DATA_ERROR_2CE8E0;
		}
	}
	if ((y -= (int)c[i]) < 0)
	{
		return Z_DATA_ERROR_2CE8E0;
	}
	c[i] += (unsigned int)y;

	// generate starting offsets into the value table for each length
	x[1] = j = 0;
	p = c + 1;
	xp = x + 2;
	while (--i) // note that i == g from above
	{
		*xp++ = (j += *p++);
	}

	// make a table of values in order of bit lengths
	p = b;
	i = 0;
	do
	{
		if ((j = *p++) != 0)
		{
			v[x[j]++] = i;
		}
	} while (++i < n);
	n = x[g]; // set n to length of v

	// generate the Huffman codes and for each, make the table entries
	x[0] = i = 0;   // first Huffman code is zero
	p = v;          // grab values in bit order
	h = -1;         // no tables yet--level -1
	w = -l;         // bits decoded == (l * h)
	u[0] = 0;
	q = 0;
	z = 0;

	// go through the bit lengths (k already is bits in shortest code)
	for (; k <= g; k++)
	{
		a = c[k];
		while (a--)
		{
			// here i is the Huffman code of length k bits for value *p;
			// make tables up to required level
			while (k > w + l)
			{
				h++;
				w += l; // previous table always l bits

				// compute minimum size table less than or equal to l bits
				z = (unsigned int)(g - w);
				z = z > (unsigned int)l ? (unsigned int)l : z; // table size upper limit
				if ((f = 1u << (j = (unsigned int)(k - w))) > a + 1) // try a k-w bit table
				{
					// too few codes for k-w bit table
					f -= a + 1; // deduct codes from patterns left
					xp = c + k;
					if (j < z)
					{
						while (++j < z) // try smaller tables up to z bits
						{
							if ((f <<= 1) <= *++xp)
							{
								break; // enough codes to use up j bits
							}
							f -= *xp; // else deduct codes from patterns
						}
					}
				}
				z = 1u << j; // table entries for j-bit table

				// allocate new table
				if (*hn + z > MANY_2CE8E0) // (note: doesn't matter for fixed)
				{
					return Z_DATA_ERROR_2CE8E0; // overflow of MANY
				}
				u[h] = q = hp + *hn;
				*hn += z;

				// connect to last table, if there is one
				if (h)
				{
					x[h] = i;                       // save pattern for backing up
					r.bits = (unsigned char)l;      // bits to dump before this table
					r.exop = (unsigned char)j;      // bits in this table
					j = i >> (w - l);
					r.base = (unsigned int)(q - u[h - 1] - j); // offset to this table
					u[h - 1][j] = r;                // connect to last table
				}
				else
				{
					*t = q; // first table is returned result
				}
			}

			// set up table entry in r
			r.bits = (unsigned char)(k - w);
			if (p >= v + n)
			{
				r.exop = 128 + 64; // out of values--invalid code
			}
			else if (*p < s)
			{
				r.exop = (unsigned char)(*p < 256 ? 0 : 32 + 64); // 256 is end-of-block
				r.base = *p++; // simple code is just the value
			}
			else
			{
				r.exop = (unsigned char)(e[*p - s] + 16 + 64); // non-simple--look up in lists
				r.base = d[*p++ - s];
			}

			// fill code-like entries with r
			f = 1u << (k - w);
			for (j = i >> w; j < z; j += f)
			{
				q[j] = r;
			}

			// backwards increment the k-bit code i
			for (j = 1u << (k - 1); i & j; j >>= 1)
			{
				i ^= j;
			}
			i ^= j;

			// backup over finished tables
			mask = (1u << w) - 1;
			while ((i & mask) != x[h])
			{
				h--; // don't need to update q
				w -= l;
				mask = (1u << w) - 1;
			}
		}
	}

	// Z_BUF_ERROR when given an incomplete table
	return y != 0 && g != 1 ? Z_BUF_ERROR_2CE8E0 : Z_OK_2CE8E0;
}

// @retail 0x2cf9e0
int __cdecl inflate_trees_bits_2cf9e0(
	unsigned int *c,                // 19 code lengths
	unsigned int *bb,               // bits tree desired/actual depth
	inflate_huft_2ce8e0 **tb,       // bits tree result
	inflate_huft_2ce8e0 *hp,        // space for trees
	z_stream_2ce8e0 *z)             // for messages
{
	unsigned int hn = 0; // hufts used in space
	unsigned int *v = (unsigned int *)ZALLOC_2CE8E0(z, 19, sizeof(unsigned int));
	if (v == 0)
	{
		return Z_MEM_ERROR_2CE8E0;
	}
	int r = huft_build_2cf4f0(c, 19, 19, 0, 0, tb, bb, hp, &hn, v);
	if (r == Z_DATA_ERROR_2CE8E0)
	{
		z->msg = (char *)"oversubscribed dynamic bit lengths tree";
	}
	else if (r == Z_BUF_ERROR_2CE8E0 || *bb == 0)
	{
		z->msg = (char *)"incomplete dynamic bit lengths tree";
		r = Z_DATA_ERROR_2CE8E0;
	}
	ZFREE_2CE8E0(z, v);
	return r;
}

// @retail 0x2cfa80
int __cdecl inflate_trees_dynamic_2cfa80(
	unsigned int nl,                // number of literal/length codes
	unsigned int nd,                // number of distance codes
	unsigned int *c,                // that many (total) code lengths
	unsigned int *bl,               // literal desired/actual bit depth
	unsigned int *bd,               // distance desired/actual bit depth
	inflate_huft_2ce8e0 **tl,       // literal/length tree result
	inflate_huft_2ce8e0 **td,       // distance tree result
	inflate_huft_2ce8e0 *hp,        // space for trees
	z_stream_2ce8e0 *z)             // for messages
{
	unsigned int hn = 0; // hufts used in space

	// allocate work area
	unsigned int *v = (unsigned int *)ZALLOC_2CE8E0(z, 288, sizeof(unsigned int));
	if (v == 0)
	{
		return Z_MEM_ERROR_2CE8E0;
	}

	// build literal/length tree
	int r = huft_build_2cf4f0(c, nl, 257, g_cplens_2ce8e0, g_cplext_2ce8e0, tl, bl, hp, &hn, v);
	if (r != Z_OK_2CE8E0 || *bl == 0)
	{
		if (r == Z_DATA_ERROR_2CE8E0)
		{
			z->msg = (char *)"oversubscribed literal/length tree";
		}
		else if (r != Z_MEM_ERROR_2CE8E0)
		{
			z->msg = (char *)"incomplete literal/length tree";
			r = Z_DATA_ERROR_2CE8E0;
		}
		ZFREE_2CE8E0(z, v);
		return r;
	}

	// build distance tree
	r = huft_build_2cf4f0(c + nl, nd, 0, g_cpdist_2ce8e0, g_cpdext_2ce8e0, td, bd, hp, &hn, v);
	if (r != Z_OK_2CE8E0 || (*bd == 0 && nl > 257))
	{
		if (r == Z_DATA_ERROR_2CE8E0)
		{
			z->msg = (char *)"oversubscribed distance tree";
		}
		else if (r == Z_BUF_ERROR_2CE8E0)
		{
			z->msg = (char *)"incomplete distance tree";
			r = Z_DATA_ERROR_2CE8E0;
		}
		else if (r != Z_MEM_ERROR_2CE8E0)
		{
			z->msg = (char *)"empty distance tree with lengths";
			r = Z_DATA_ERROR_2CE8E0;
		}
		ZFREE_2CE8E0(z, v);
		return r;
	}

	ZFREE_2CE8E0(z, v);
	return Z_OK_2CE8E0;
}

// inflate_trees_fixed, inlined into inflate_blocks
static __forceinline int inflate_trees_fixed_2ce8e0(unsigned int *bl, unsigned int *bd, inflate_huft_2ce8e0 **tl,
	inflate_huft_2ce8e0 **td)
{
	*bl = g_475de8;
	*bd = g_475dec;
	*tl = (inflate_huft_2ce8e0 *)g_475df0;
	*td = (inflate_huft_2ce8e0 *)g_476df0;
	return Z_OK_2CE8E0;
}

// ---------------------------------------------------------------------------
// inffast.c

// get bytes and bits without checking for the end of the input; give back
// the whole bytes not used
#define GRABBITS(j) {while (k < (j)) {b |= ((unsigned long)NEXTBYTE) << k; k += 8;}}
#define UNGRAB {c = z->avail_in - n; c = (k >> 3) < c ? k >> 3 : c; n += c; p -= c; k -= c << 3;}

// Called with number of bytes left to write in window at least 258 (the
// maximum string length) and number of input bytes available at least ten.
// The ten bytes are six bytes for the longest length/distance pair plus four
// bytes for overloading the bit buffer.
// @retail 0x2d0460
int __cdecl inflate_fast_2d0460(unsigned int bl, unsigned int bd, inflate_huft_2ce8e0 *tl, inflate_huft_2ce8e0 *td,
	inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z)
{
	inflate_huft_2ce8e0 *t; // temporary pointer
	unsigned int e;         // extra bits or operation
	unsigned long b;        // bit buffer
	unsigned int k;         // bits in bit buffer
	unsigned char *p;       // input data pointer
	unsigned int n;         // bytes available there
	unsigned char *q;       // output window write pointer
	unsigned int m;         // bytes to end of window or read pointer
	unsigned int ml;        // mask for literal/length tree
	unsigned int md;        // mask for distance tree
	unsigned int c;         // bytes to copy
	unsigned int d;         // distance back to copy from
	unsigned char *r;       // copy source pointer

	// load input, output, bit values
	LOAD

	// initialize masks
	ml = g_inflate_mask_2ce8e0[bl];
	md = g_inflate_mask_2ce8e0[bd];

	// do until not enough input or output space for fast loop
	do // assume called with m >= 258 && n >= 10
	{
		// get literal/length code
		GRABBITS(20) // max bits for literal/length code
		if ((e = (t = tl + ((unsigned int)b & ml))->exop) == 0)
		{
			DUMPBITS(t->bits)
			*q++ = (unsigned char)t->base;
			m--;
			continue;
		}
		do
		{
			DUMPBITS(t->bits)
			if (e & 16)
			{
				// get extra bits for length
				e &= 15;
				c = t->base + ((unsigned int)b & g_inflate_mask_2ce8e0[e]);
				DUMPBITS(e)

				// decode distance base of block to copy
				GRABBITS(15) // max bits for distance code
				e = (t = td + ((unsigned int)b & md))->exop;
				do
				{
					DUMPBITS(t->bits)
					if (e & 16)
					{
						// get extra bits to add to distance base
						e &= 15;
						GRABBITS(e) // get extra bits (up to 13)
						d = t->base + ((unsigned int)b & g_inflate_mask_2ce8e0[e]);
						DUMPBITS(e)

						// do the copy
						m -= c;
						r = q - d;
						if (r < s->window) // wrap if needed
						{
							do
							{
								r += s->end - s->window; // force pointer in window
							} while (r < s->window);    // covers invalid distances
							e = (unsigned int)(s->end - r);
							if (c > e)
							{
								c -= e; // wrapped copy
								do
								{
									*q++ = *r++;
								} while (--e);
								r = s->window;
								do
								{
									*q++ = *r++;
								} while (--c);
							}
							else // normal copy
							{
								*q++ = *r++;
								c--;
								*q++ = *r++;
								c--;
								do
								{
									*q++ = *r++;
								} while (--c);
							}
						}
						else // normal copy
						{
							*q++ = *r++;
							c--;
							*q++ = *r++;
							c--;
							do
							{
								*q++ = *r++;
							} while (--c);
						}
						break;
					}
					else if ((e & 64) == 0)
					{
						t += t->base;
						e = (t += ((unsigned int)b & g_inflate_mask_2ce8e0[e]))->exop;
					}
					else
					{
						z->msg = (char *)"invalid distance code";
						UNGRAB
						UPDATE
						return Z_DATA_ERROR_2CE8E0;
					}
				} while (1);
				break;
			}
			if ((e & 64) == 0)
			{
				t += t->base;
				if ((e = (t += ((unsigned int)b & g_inflate_mask_2ce8e0[e]))->exop) == 0)
				{
					DUMPBITS(t->bits)
					*q++ = (unsigned char)t->base;
					m--;
					break;
				}
			}
			else if (e & 32)
			{
				UNGRAB
				UPDATE
				return Z_STREAM_END_2CE8E0;
			}
			else
			{
				z->msg = (char *)"invalid literal/length code";
				UNGRAB
				UPDATE
				return Z_DATA_ERROR_2CE8E0;
			}
		} while (1);
	} while (m >= 258 && n >= 10);

	// not enough input or output--restore pointers and return
	UNGRAB
	UPDATE
	return Z_OK_2CE8E0;
}

#undef GRABBITS
#undef UNGRAB

// ---------------------------------------------------------------------------
// infcodes.c

// @retail 0x2cfd30
inflate_codes_state_2ce8e0 *__cdecl inflate_codes_new_2cfd30(unsigned int bl, unsigned int bd,
	inflate_huft_2ce8e0 *tl, inflate_huft_2ce8e0 *td, z_stream_2ce8e0 *z)
{
	inflate_codes_state_2ce8e0 *c =
		(inflate_codes_state_2ce8e0 *)ZALLOC_2CE8E0(z, 1, sizeof(inflate_codes_state_2ce8e0));
	if (c != 0)
	{
		c->mode = START_2CE8E0;
		c->lbits = (unsigned char)bl;
		c->dbits = (unsigned char)bd;
		c->ltree = tl;
		c->dtree = td;
	}
	return c;
}

// @retail 0x2cfd70
int __cdecl inflate_codes_2cfd70(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z, int r)
{
	unsigned int j;          // temporary storage
	inflate_huft_2ce8e0 *t;  // temporary pointer
	unsigned int e;          // extra bits or operation
	unsigned long b;         // bit buffer
	unsigned int k;          // bits in bit buffer
	unsigned char *p;        // input data pointer
	unsigned int n;          // bytes available there
	unsigned char *q;        // output window write pointer
	unsigned int m;          // bytes to end of window or read pointer
	unsigned char *f;        // pointer to copy strings from
	inflate_codes_state_2ce8e0 *c = s->sub.decode.codes; // codes state

	// copy input/output information to locals (UPDATE macro restores)
	LOAD

	// process input and output based on current state
	while (1)
	{
		switch (c->mode)
		{
		// waiting for "i:"=input, "o:"=output, "x:"=nothing
		case START_2CE8E0: // x: set up for LEN
			if (m >= 258 && n >= 10)
			{
				UPDATE
				r = inflate_fast_2d0460(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
				LOAD
				if (r != Z_OK_2CE8E0)
				{
					c->mode = r == Z_STREAM_END_2CE8E0 ? WASH_2CE8E0 : BADCODE_2CE8E0;
					break;
				}
			}
			c->sub.code.need = c->lbits;
			c->sub.code.tree = c->ltree;
			c->mode = LEN_2CE8E0;
			// fall through
		case LEN_2CE8E0: // i: get length/literal/eob next
			j = c->sub.code.need;
			NEEDBITS(j)
			t = c->sub.code.tree + ((unsigned int)b & g_inflate_mask_2ce8e0[j]);
			DUMPBITS(t->bits)
			e = (unsigned int)(t->exop);
			if (e == 0) // literal
			{
				c->sub.lit = t->base;
				c->mode = LIT_2CE8E0;
				break;
			}
			if (e & 16) // length
			{
				c->sub.copy.get = e & 15;
				c->len = t->base;
				c->mode = LENEXT_2CE8E0;
				break;
			}
			if ((e & 64) == 0) // next table
			{
				c->sub.code.need = e;
				c->sub.code.tree = t + t->base;
				break;
			}
			if (e & 32) // end of block
			{
				c->mode = WASH_2CE8E0;
				break;
			}
			c->mode = BADCODE_2CE8E0; // invalid code
			z->msg = (char *)"invalid literal/length code";
			r = Z_DATA_ERROR_2CE8E0;
			LEAVE
		case LENEXT_2CE8E0: // i: getting length extra (have base)
			j = c->sub.copy.get;
			NEEDBITS(j)
			c->len += (unsigned int)b & g_inflate_mask_2ce8e0[j];
			DUMPBITS(j)
			c->sub.code.need = c->dbits;
			c->sub.code.tree = c->dtree;
			c->mode = DIST_2CE8E0;
			// fall through
		case DIST_2CE8E0: // i: get distance next
			j = c->sub.code.need;
			NEEDBITS(j)
			t = c->sub.code.tree + ((unsigned int)b & g_inflate_mask_2ce8e0[j]);
			DUMPBITS(t->bits)
			e = (unsigned int)(t->exop);
			if (e & 16) // distance
			{
				c->sub.copy.get = e & 15;
				c->sub.copy.dist = t->base;
				c->mode = DISTEXT_2CE8E0;
				break;
			}
			if ((e & 64) == 0) // next table
			{
				c->sub.code.need = e;
				c->sub.code.tree = t + t->base;
				break;
			}
			c->mode = BADCODE_2CE8E0; // invalid code
			z->msg = (char *)"invalid distance code";
			r = Z_DATA_ERROR_2CE8E0;
			LEAVE
		case DISTEXT_2CE8E0: // i: getting distance extra
			j = c->sub.copy.get;
			NEEDBITS(j)
			c->sub.copy.dist += (unsigned int)b & g_inflate_mask_2ce8e0[j];
			DUMPBITS(j)
			c->mode = COPY_2CE8E0;
			// fall through
		case COPY_2CE8E0: // o: copying bytes in window, waiting for space
			f = q - c->sub.copy.dist;
			while (f < s->window) // modulo window size-"while" instead
			{
				f += s->end - s->window; // of "if" handles invalid distances
			}
			while (c->len)
			{
				NEEDOUT
				OUTBYTE(*f++)
				if (f == s->end)
				{
					f = s->window;
				}
				c->len--;
			}
			c->mode = START_2CE8E0;
			break;
		case LIT_2CE8E0: // o: got literal, waiting for output space
			NEEDOUT
			OUTBYTE(c->sub.lit)
			c->mode = START_2CE8E0;
			break;
		case WASH_2CE8E0: // o: got eob, possibly more output
			if (k > 7) // return unused byte, if any
			{
				k -= 8;
				n++;
				p--; // can always return one
			}
			FLUSH
			if (s->read != s->write)
			{
				LEAVE
			}
			c->mode = END_2CE8E0;
			// fall through
		case END_2CE8E0:
			r = Z_STREAM_END_2CE8E0;
			LEAVE
		case BADCODE_2CE8E0: // x: got error
			r = Z_DATA_ERROR_2CE8E0;
			LEAVE
		default:
			r = Z_STREAM_ERROR_2CE8E0;
			LEAVE
		}
	}
}

// ---------------------------------------------------------------------------
// infblock.c

// @retail 0x2ce8e0
void __cdecl inflate_blocks_reset_2ce8e0(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z, unsigned long *c)
{
	if (c != 0)
	{
		*c = s->check;
	}
	if (s->mode == BTREE_2CE8E0 || s->mode == DTREE_2CE8E0)
	{
		ZFREE_2CE8E0(z, s->sub.trees.blens);
	}
	if (s->mode == CODES_2CE8E0)
	{
		// inflate_codes_free
		ZFREE_2CE8E0(z, s->sub.decode.codes);
	}
	s->mode = TYPE_2CE8E0;
	s->bitk = 0;
	s->bitb = 0;
	s->read = s->write = s->window;
	if (s->checkfn != 0)
	{
		z->adler = s->check = (*s->checkfn)(0L, 0, 0);
	}
}

// @retail 0x2ce950
inflate_blocks_state_2ce8e0 *__cdecl inflate_blocks_new_2ce950(z_stream_2ce8e0 *z, check_func_2ce8e0 c,
	unsigned int w)
{
	inflate_blocks_state_2ce8e0 *s =
		(inflate_blocks_state_2ce8e0 *)ZALLOC_2CE8E0(z, 1, sizeof(inflate_blocks_state_2ce8e0));
	if (s == 0)
	{
		return s;
	}
	if ((s->hufts = (inflate_huft_2ce8e0 *)ZALLOC_2CE8E0(z, sizeof(inflate_huft_2ce8e0), MANY_2CE8E0)) == 0)
	{
		ZFREE_2CE8E0(z, s);
		return 0;
	}
	if ((s->window = (unsigned char *)ZALLOC_2CE8E0(z, 1, w)) == 0)
	{
		ZFREE_2CE8E0(z, s->hufts);
		ZFREE_2CE8E0(z, s);
		return 0;
	}
	s->end = s->window + w;
	s->checkfn = c;
	s->mode = TYPE_2CE8E0;
	inflate_blocks_reset_2ce8e0(s, z, 0);
	return s;
}

// @retail 0x2ce9e0
int __cdecl inflate_blocks_2ce9e0(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z, int r)
{
	unsigned int t;    // temporary storage
	unsigned long b;   // bit buffer
	unsigned int k;    // bits in bit buffer
	unsigned char *p;  // input data pointer
	unsigned int n;    // bytes available there
	unsigned char *q;  // output window write pointer
	unsigned int m;    // bytes to end of window or read pointer

	// copy input/output information to locals (UPDATE macro restores)
	LOAD

	// process input based on current state
	while (1)
	{
		switch (s->mode)
		{
		case TYPE_2CE8E0:
			NEEDBITS(3)
			t = (unsigned int)b & 7;
			s->last = t & 1;
			switch (t >> 1)
			{
			case 0: // stored
				DUMPBITS(3)
				t = k & 7; // go to byte boundary
				DUMPBITS(t)
				s->mode = LENS_2CE8E0; // get length of stored block
				break;
			case 1: // fixed
			{
				unsigned int bl, bd;
				inflate_huft_2ce8e0 *tl, *td;

				inflate_trees_fixed_2ce8e0(&bl, &bd, &tl, &td);
				s->sub.decode.codes = inflate_codes_new_2cfd30(bl, bd, tl, td, z);
				if (s->sub.decode.codes == 0)
				{
					r = Z_MEM_ERROR_2CE8E0;
					LEAVE
				}
			}
				DUMPBITS(3)
				s->mode = CODES_2CE8E0;
				break;
			case 2: // dynamic
				DUMPBITS(3)
				s->mode = TABLE_2CE8E0;
				break;
			case 3: // illegal
				DUMPBITS(3)
				s->mode = BAD_2CE8E0;
				z->msg = (char *)"invalid block type";
				r = Z_DATA_ERROR_2CE8E0;
				LEAVE
			}
			break;
		case LENS_2CE8E0:
			NEEDBITS(32)
			if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
			{
				s->mode = BAD_2CE8E0;
				z->msg = (char *)"invalid stored block lengths";
				r = Z_DATA_ERROR_2CE8E0;
				LEAVE
			}
			s->sub.left = (unsigned int)b & 0xffff;
			b = k = 0; // dump bits
			s->mode = s->sub.left ? STORED_2CE8E0 : (s->last ? DRY_2CE8E0 : TYPE_2CE8E0);
			break;
		case STORED_2CE8E0:
			if (n == 0)
			{
				LEAVE
			}
			NEEDOUT
			t = s->sub.left;
			if (t > n)
			{
				t = n;
			}
			if (t > m)
			{
				t = m;
			}
			memcpy(q, p, t);
			p += t;
			n -= t;
			q += t;
			m -= t;
			if ((s->sub.left -= t) != 0)
			{
				break;
			}
			s->mode = s->last ? DRY_2CE8E0 : TYPE_2CE8E0;
			break;
		case TABLE_2CE8E0:
			NEEDBITS(14)
			s->sub.trees.table = t = (unsigned int)b & 0x3fff;
			if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
			{
				s->mode = BAD_2CE8E0;
				z->msg = (char *)"too many length or distance symbols";
				r = Z_DATA_ERROR_2CE8E0;
				LEAVE
			}
			t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
			if ((s->sub.trees.blens = (unsigned int *)ZALLOC_2CE8E0(z, t, sizeof(unsigned int))) == 0)
			{
				r = Z_MEM_ERROR_2CE8E0;
				LEAVE
			}
			DUMPBITS(14)
			s->sub.trees.index = 0;
			s->mode = BTREE_2CE8E0;
			// fall through
		case BTREE_2CE8E0:
			while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
			{
				NEEDBITS(3)
				s->sub.trees.blens[g_border_2ce8e0[s->sub.trees.index++]] = (unsigned int)b & 7;
				DUMPBITS(3)
			}
			while (s->sub.trees.index < 19)
			{
				s->sub.trees.blens[g_border_2ce8e0[s->sub.trees.index++]] = 0;
			}
			s->sub.trees.bb = 7;
			t = (unsigned int)inflate_trees_bits_2cf9e0(s->sub.trees.blens, &s->sub.trees.bb, &s->sub.trees.tb,
				s->hufts, z);
			if (t != Z_OK_2CE8E0)
			{
				r = (int)t;
				if (r == Z_DATA_ERROR_2CE8E0)
				{
					ZFREE_2CE8E0(z, s->sub.trees.blens);
					s->mode = BAD_2CE8E0;
				}
				LEAVE
			}
			s->sub.trees.index = 0;
			s->mode = DTREE_2CE8E0;
			// fall through
		case DTREE_2CE8E0:
			while (t = s->sub.trees.table, s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
			{
				inflate_huft_2ce8e0 *h;
				unsigned int i, j, c;

				t = s->sub.trees.bb;
				NEEDBITS(t)
				h = s->sub.trees.tb + ((unsigned int)b & g_inflate_mask_2ce8e0[t]);
				t = h->bits;
				c = h->base;
				if (c < 16)
				{
					DUMPBITS(t)
					s->sub.trees.blens[s->sub.trees.index++] = c;
				}
				else // c == 16..18
				{
					i = c == 18 ? 7 : c - 14;
					j = c == 18 ? 11 : 3;
					NEEDBITS(t + i)
					DUMPBITS(t)
					j += (unsigned int)b & g_inflate_mask_2ce8e0[i];
					DUMPBITS(i)
					i = s->sub.trees.index;
					t = s->sub.trees.table;
					if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) || (c == 16 && i < 1))
					{
						ZFREE_2CE8E0(z, s->sub.trees.blens);
						s->mode = BAD_2CE8E0;
						z->msg = (char *)"invalid bit length repeat";
						r = Z_DATA_ERROR_2CE8E0;
						LEAVE
					}
					c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
					do
					{
						s->sub.trees.blens[i++] = c;
					} while (--j);
					s->sub.trees.index = i;
				}
			}
			s->sub.trees.tb = 0;
			{
				unsigned int bl, bd;
				inflate_huft_2ce8e0 *tl, *td;
				inflate_codes_state_2ce8e0 *c;

				bl = 9; // must be <= 9 for lookahead assumptions
				bd = 6; // must be <= 9 for lookahead assumptions
				t = s->sub.trees.table;
				t = (unsigned int)inflate_trees_dynamic_2cfa80(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
					s->sub.trees.blens, &bl, &bd, &tl, &td, s->hufts, z);
				if (t != Z_OK_2CE8E0)
				{
					if (t == (unsigned int)Z_DATA_ERROR_2CE8E0)
					{
						ZFREE_2CE8E0(z, s->sub.trees.blens);
						s->mode = BAD_2CE8E0;
					}
					r = (int)t;
					LEAVE
				}
				if ((c = inflate_codes_new_2cfd30(bl, bd, tl, td, z)) == 0)
				{
					r = Z_MEM_ERROR_2CE8E0;
					LEAVE
				}
				s->sub.decode.codes = c;
			}
			ZFREE_2CE8E0(z, s->sub.trees.blens);
			s->mode = CODES_2CE8E0;
			// fall through
		case CODES_2CE8E0:
			UPDATE
			if ((r = inflate_codes_2cfd70(s, z, r)) != Z_STREAM_END_2CE8E0)
			{
				return inflate_flush_2cfbf0(s, z, r);
			}
			r = Z_OK_2CE8E0;
			// inflate_codes_free
			ZFREE_2CE8E0(z, s->sub.decode.codes);
			LOAD
			if (!s->last)
			{
				s->mode = TYPE_2CE8E0;
				break;
			}
			s->mode = DRY_2CE8E0;
			// fall through
		case DRY_2CE8E0:
			FLUSH
			if (s->read != s->write)
			{
				LEAVE
			}
			s->mode = DONE_2CE8E0;
			// fall through
		case DONE_2CE8E0:
			r = Z_STREAM_END_2CE8E0;
			LEAVE
		case BAD_2CE8E0:
			r = Z_DATA_ERROR_2CE8E0;
			LEAVE
		default:
			r = Z_STREAM_ERROR_2CE8E0;
			LEAVE
		}
	}
}

// @retail 0x2cf480
int __cdecl inflate_blocks_free_2cf480(inflate_blocks_state_2ce8e0 *s, z_stream_2ce8e0 *z)
{
	inflate_blocks_reset_2ce8e0(s, z, 0);
	ZFREE_2CE8E0(z, s->window);
	ZFREE_2CE8E0(z, s->hufts);
	ZFREE_2CE8E0(z, s);
	return Z_OK_2CE8E0;
}
