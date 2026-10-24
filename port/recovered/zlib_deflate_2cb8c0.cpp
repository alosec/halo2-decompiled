// @flags /O2 /GL-
// zlib 1.1.4 (third-party compression library, linked into the game): the
// rest of deflate.c, after deflateInit2_/deflateReset
// (zlib_deflate_init_2cb640.cpp):
//   0x2cb8c0  putShortMSB      0x2cbd00  lm_init
//   0x2cb8f0  flush_pending    0x2cbd90  longest_match
//   0x2cb960  deflate          0x2cbf00  fill_window
//   0x2cbbf0  deflateEnd       0x2cc020  deflate_stored
//   0x2cbc90  read_buf         0x2cc170  deflate_fast
//                              0x2cc470  deflate_slow
// Stock 1.1.4 (no ASMV, no UNALIGNED_OK, not FASTEST, no DEBUG, so
// _tr_tally is the inline macro). deflateSetDictionary, deflateParams and
// deflateCopy are not in the image.
//
// Whole-program optimization moved arguments into registers; here every
// function takes zlib's own parameters in zlib's order. Retail's registers:
//   putShortMSB     s eax, b ecx
//   flush_pending   strm eax
//   deflate         strm ebx (flush on the stack)
//   deflateEnd      strm esi
//   read_buf        size ecx (strm, buf on the stack)
//   lm_init         s edx
//   longest_match   s esi, cur_match eax
//   fill_window     s ebx
// (deflate_stored/fast/slow are reached through configuration_table, so they
// keep the stack: s, flush, callee pops.)
//
// The configuration table (retail 0x44f970, 12-byte entries ending in the
// block function) is kept here as code-recovered data; its values match
// retail's. z_errmsg (retail 0x475dc0) is replaced by its strings.
#include <string.h>
#include "zlib_deflate_2cb8c0.h"

enum
{
	Z_NO_FLUSH_2CB8C0 = 0,
	Z_PARTIAL_FLUSH_2CB8C0 = 1,
	Z_SYNC_FLUSH_2CB8C0 = 2,
	Z_FULL_FLUSH_2CB8C0 = 3,
	Z_FINISH_2CB8C0 = 4,
	Z_OK_2CB8C0 = 0,
	Z_STREAM_END_2CB8C0 = 1,
	Z_STREAM_ERROR_2CB8C0 = -2,
	Z_DATA_ERROR_2CB8C0 = -3,
	Z_BUF_ERROR_2CB8C0 = -5,
	Z_FILTERED_2CB8C0 = 1,
	Z_HUFFMAN_ONLY_2CB8C0 = 2,
	Z_DEFLATED_2CB8C0 = 8,
	PRESET_DICT_2CB8C0 = 0x20,
	INIT_STATE_2CB8C0 = 42,
	BUSY_STATE_2CB8C0 = 113,
	FINISH_STATE_2CB8C0 = 666,
	NIL_2CB8C0 = 0,
	TOO_FAR_2CB8C0 = 4096
};

// what a block function reports
enum block_state_2cb8c0
{
	need_more_2cb8c0,      // block not completed, need more input or more output
	block_done_2cb8c0,     // block flush performed
	finish_started_2cb8c0, // finish started, need only more output at next deflate
	finish_done_2cb8c0     // finish done, accept no more input or output
};

typedef block_state_2cb8c0 (__stdcall *compress_func_2cb8c0)(deflate_state_2cb640 *s, int flush);

block_state_2cb8c0 __stdcall deflate_stored_2cc020(deflate_state_2cb640 *s, int flush);
block_state_2cb8c0 __stdcall deflate_fast_2cc170(deflate_state_2cb640 *s, int flush);
block_state_2cb8c0 __stdcall deflate_slow_2cc470(deflate_state_2cb640 *s, int flush);

struct config_2cb8c0
{
	unsigned short good_length; // reduce lazy search above this match length
	unsigned short max_lazy;    // do not perform lazy search above this match length
	unsigned short nice_length; // quit search above this match length
	unsigned short max_chain;
	compress_func_2cb8c0 func;
};

// configuration_table (retail 0x44f970)
static config_2cb8c0 const g_configuration_table_2cb8c0[10] = {
	// good lazy nice chain
	{0, 0, 0, 0, deflate_stored_2cc020},       // 0: store only
	{4, 4, 8, 4, deflate_fast_2cc170},         // 1: maximum speed, no lazy matches
	{4, 5, 16, 8, deflate_fast_2cc170},
	{4, 6, 32, 32, deflate_fast_2cc170},
	{4, 4, 16, 16, deflate_slow_2cc470},       // 4: lazy matches
	{8, 16, 32, 32, deflate_slow_2cc470},
	{8, 16, 128, 128, deflate_slow_2cc470},
	{8, 32, 128, 256, deflate_slow_2cc470},
	{32, 128, 258, 1024, deflate_slow_2cc470},
	{32, 258, 258, 4096, deflate_slow_2cc470}}; // 9: maximum compression

// z_errmsg's entries (retail 0x475dc0)
static char const g_stream_error_message_2cb8c0[] = "stream error";
static char const g_buffer_error_message_2cb8c0[] = "buffer error";

#define MAX_DIST_2CB8C0(s) ((s)->w_size - MIN_LOOKAHEAD_2CB8C0)
#define UPDATE_HASH_2CB8C0(s, h, c) (h = (((h) << (s)->hash_shift) ^ (c)) & (s)->hash_mask)
#define INSERT_STRING_2CB8C0(s, str, match_head) \
	(UPDATE_HASH_2CB8C0(s, s->ins_h, s->window[(str) + (MIN_MATCH_2CB8C0 - 1)]), \
	 s->prev[(str) & s->w_mask] = match_head = s->head[s->ins_h], \
	 s->head[s->ins_h] = (unsigned short)(str))
#define CLEAR_HASH_2CB8C0(s) \
	s->head[s->hash_size - 1] = NIL_2CB8C0; \
	memset((unsigned char *)s->head, 0, (unsigned int)(s->hash_size - 1) * sizeof(*s->head));
#define d_code_2cb8c0(dist) \
	((dist) < 256 ? g_dist_code_2ccdc0[dist] : g_dist_code_2ccdc0[256 + ((dist) >> 7)])
#define tr_tally_lit_2cb8c0(s, c, flush) \
	{ \
		unsigned char cc = (c); \
		s->d_buf[s->last_lit] = 0; \
		s->l_buf[s->last_lit++] = cc; \
		s->dyn_ltree[cc].freq_or_code++; \
		flush = (s->last_lit == s->lit_bufsize - 1); \
	}
#define tr_tally_dist_2cb8c0(s, distance, length, flush) \
	{ \
		unsigned char len = (unsigned char)(length); \
		unsigned short dist = (unsigned short)(distance); \
		s->d_buf[s->last_lit] = dist; \
		s->l_buf[s->last_lit++] = len; \
		dist--; \
		s->dyn_ltree[g_length_code_2ccdc0[len] + LITERALS_2CB8C0 + 1].freq_or_code++; \
		s->dyn_dtree[d_code_2cb8c0(dist)].freq_or_code++; \
		flush = (s->last_lit == s->lit_bufsize - 1); \
	}

// puts a short in the pending buffer, most significant byte first
// @retail 0x2cb8c0
void __cdecl put_short_msb_2cb8c0(deflate_state_2cb640 *s, unsigned int b)
{
	s->pending_buf[s->pending++] = (unsigned char)(b >> 8);
	s->pending_buf[s->pending++] = (unsigned char)(b & 0xff);
}

// flushes as much pending output as possible
// @retail 0x2cb8f0
void __cdecl flush_pending_2cb8f0(z_stream_2cb640 *strm)
{
	unsigned int len = (unsigned int)strm->state->pending;
	if (len > strm->avail_out)
	{
		len = strm->avail_out;
	}
	if (len == 0)
	{
		return;
	}
	memcpy(strm->next_out, strm->state->pending_out, len);
	strm->next_out += len;
	strm->state->pending_out += len;
	strm->total_out += len;
	strm->avail_out -= len;
	strm->state->pending -= (int)len;
	if (strm->state->pending == 0)
	{
		strm->state->pending_out = strm->state->pending_buf;
	}
}

// @retail 0x2cb960
long __cdecl deflate_2cb960(z_stream_2cb640 *strm, long flush)
{
	if (strm == 0 || strm->state == 0 || flush > Z_FINISH_2CB8C0 || flush < 0)
	{
		return Z_STREAM_ERROR_2CB8C0;
	}
	deflate_state_2cb640 *s = strm->state;

	if (strm->next_out == 0 || (strm->next_in == 0 && strm->avail_in != 0) ||
		(s->status == FINISH_STATE_2CB8C0 && flush != Z_FINISH_2CB8C0))
	{
		strm->msg = (char *)g_stream_error_message_2cb8c0;
		return Z_STREAM_ERROR_2CB8C0;
	}
	if (strm->avail_out == 0)
	{
		strm->msg = (char *)g_buffer_error_message_2cb8c0;
		return Z_BUF_ERROR_2CB8C0;
	}

	s->strm = strm; // just in case
	int old_flush = s->last_flush;
	s->last_flush = (int)flush;

	// write the zlib header
	if (s->status == INIT_STATE_2CB8C0)
	{
		unsigned int header = (Z_DEFLATED_2CB8C0 + ((s->w_bits - 8) << 4)) << 8;
		unsigned int level_flags = (unsigned int)(s->level - 1) >> 1;
		if (level_flags > 3)
		{
			level_flags = 3;
		}
		header |= (level_flags << 6);
		if (s->strstart != 0)
		{
			header |= PRESET_DICT_2CB8C0;
		}
		header += 31 - (header % 31);

		s->status = BUSY_STATE_2CB8C0;
		put_short_msb_2cb8c0(s, header);

		// save the adler32 of the preset dictionary
		if (s->strstart != 0)
		{
			put_short_msb_2cb8c0(s, (unsigned int)(strm->adler >> 16));
			put_short_msb_2cb8c0(s, (unsigned int)(strm->adler & 0xffff));
		}
		strm->adler = 1L;
	}

	// flush as much pending output as possible
	if (s->pending != 0)
	{
		flush_pending_2cb8f0(strm);
		if (strm->avail_out == 0)
		{
			// deflate will be called again with more output space; make it
			// return OK rather than BUF_ERROR then
			s->last_flush = -1;
			return Z_OK_2CB8C0;
		}
	}
	// make sure there is something to do and avoid duplicate consecutive
	// flushes (repeated Z_FINISH keeps returning Z_STREAM_END)
	else if (strm->avail_in == 0 && flush <= old_flush && flush != Z_FINISH_2CB8C0)
	{
		strm->msg = (char *)g_buffer_error_message_2cb8c0;
		return Z_BUF_ERROR_2CB8C0;
	}

	// no more input after the first FINISH
	if (s->status == FINISH_STATE_2CB8C0 && strm->avail_in != 0)
	{
		strm->msg = (char *)g_buffer_error_message_2cb8c0;
		return Z_BUF_ERROR_2CB8C0;
	}

	// start a new block or continue the current one
	if (strm->avail_in != 0 || s->lookahead != 0 ||
		(flush != Z_NO_FLUSH_2CB8C0 && s->status != FINISH_STATE_2CB8C0))
	{
		block_state_2cb8c0 bstate = (*(g_configuration_table_2cb8c0[s->level].func))(s, (int)flush);

		if (bstate == finish_started_2cb8c0 || bstate == finish_done_2cb8c0)
		{
			s->status = FINISH_STATE_2CB8C0;
		}
		if (bstate == need_more_2cb8c0 || bstate == finish_started_2cb8c0)
		{
			if (strm->avail_out == 0)
			{
				s->last_flush = -1; // avoid BUF_ERROR next call
			}
			return Z_OK_2CB8C0;
		}
		if (bstate == block_done_2cb8c0)
		{
			if (flush == Z_PARTIAL_FLUSH_2CB8C0)
			{
				tr_align_2cdd80(s);
			}
			else
			{
				// FULL_FLUSH or SYNC_FLUSH: an empty stored block
				tr_stored_block_2cdce0(s, (char *)0, 0L, 0);
				if (flush == Z_FULL_FLUSH_2CB8C0)
				{
					CLEAR_HASH_2CB8C0(s); // forget history
				}
			}
			flush_pending_2cb8f0(strm);
			if (strm->avail_out == 0)
			{
				s->last_flush = -1; // avoid BUF_ERROR at next call
				return Z_OK_2CB8C0;
			}
		}
	}

	if (flush != Z_FINISH_2CB8C0)
	{
		return Z_OK_2CB8C0;
	}
	if (s->noheader)
	{
		return Z_STREAM_END_2CB8C0;
	}

	// write the zlib trailer (adler32)
	put_short_msb_2cb8c0(s, (unsigned int)(strm->adler >> 16));
	put_short_msb_2cb8c0(s, (unsigned int)(strm->adler & 0xffff));
	flush_pending_2cb8f0(strm);
	s->noheader = -1; // write the trailer only once
	return s->pending != 0 ? Z_OK_2CB8C0 : Z_STREAM_END_2CB8C0;
}

// @retail 0x2cbbf0
long __cdecl deflate_end_2cbbf0(z_stream_2cb640 *strm)
{
	if (strm == 0 || strm->state == 0)
	{
		return Z_STREAM_ERROR_2CB8C0;
	}
	int status = strm->state->status;
	if (status != INIT_STATE_2CB8C0 && status != BUSY_STATE_2CB8C0 && status != FINISH_STATE_2CB8C0)
	{
		return Z_STREAM_ERROR_2CB8C0;
	}

	// deallocate in reverse order of allocations
	if (strm->state->pending_buf)
	{
		strm->zfree(strm->opaque, strm->state->pending_buf);
	}
	if (strm->state->head)
	{
		strm->zfree(strm->opaque, strm->state->head);
	}
	if (strm->state->prev)
	{
		strm->zfree(strm->opaque, strm->state->prev);
	}
	if (strm->state->window)
	{
		strm->zfree(strm->opaque, strm->state->window);
	}
	strm->zfree(strm->opaque, strm->state);
	strm->state = 0;

	return status == BUSY_STATE_2CB8C0 ? Z_DATA_ERROR_2CB8C0 : Z_OK_2CB8C0;
}

// reads a new buffer from the current input stream, updating the adler32
// and the total number of bytes read
// @retail 0x2cbc90
int __cdecl read_buf_2cbc90(z_stream_2cb640 *strm, unsigned char *buf, unsigned int size)
{
	unsigned int len = strm->avail_in;
	if (len > size)
	{
		len = size;
	}
	if (len == 0)
	{
		return 0;
	}
	strm->avail_in -= len;
	if (!strm->state->noheader)
	{
		strm->adler = adler32_2ce7b0(strm->adler, strm->next_in, len);
	}
	memcpy(buf, strm->next_in, len);
	strm->next_in += len;
	strm->total_in += len;
	return (int)len;
}

// initializes the "longest match" routines for a new zlib stream
// @retail 0x2cbd00
void __cdecl lm_init_2cbd00(deflate_state_2cb640 *s)
{
	s->window_size = (unsigned long)2L * s->w_size;

	CLEAR_HASH_2CB8C0(s);

	// the default configuration parameters
	s->max_lazy_match = g_configuration_table_2cb8c0[s->level].max_lazy;
	s->good_match = g_configuration_table_2cb8c0[s->level].good_length;
	s->nice_match = g_configuration_table_2cb8c0[s->level].nice_length;
	s->max_chain_length = g_configuration_table_2cb8c0[s->level].max_chain;

	s->strstart = 0;
	s->block_start = 0L;
	s->lookahead = 0;
	s->match_length = s->prev_length = MIN_MATCH_2CB8C0 - 1;
	s->match_available = 0;
	s->ins_h = 0;
}

// sets match_start to the longest match starting at the given string and
// returns its length (matches no longer than prev_length are discarded)
// @retail 0x2cbd90
unsigned int __cdecl longest_match_2cbd90(deflate_state_2cb640 *s, unsigned int cur_match)
{
	unsigned int chain_length = s->max_chain_length; // max hash chain length
	unsigned char *scan = s->window + s->strstart;   // current string
	unsigned char *match;                            // matched string
	int len;                                         // length of current match
	int best_len = (int)s->prev_length;              // best match length so far
	int nice_match = s->nice_match;                  // stop if match long enough
	unsigned int limit = s->strstart > (unsigned int)MAX_DIST_2CB8C0(s) ?
		s->strstart - (unsigned int)MAX_DIST_2CB8C0(s) : NIL_2CB8C0;
	unsigned short *prev = s->prev;
	unsigned int wmask = s->w_mask;

	unsigned char *strend = s->window + s->strstart + MAX_MATCH_2CB8C0;
	unsigned char scan_end1 = scan[best_len - 1];
	unsigned char scan_end = scan[best_len];

	// do not waste too much time if we already have a good match
	if (s->prev_length >= s->good_match)
	{
		chain_length >>= 2;
	}
	// do not look for matches beyond the end of the input
	if ((unsigned int)nice_match > s->lookahead)
	{
		nice_match = (int)s->lookahead;
	}

	do
	{
		match = s->window + cur_match;

		// skip to the next match if the length cannot increase or the
		// match is shorter than 2
		if (match[best_len] != scan_end || match[best_len - 1] != scan_end1 || *match != *scan ||
			*++match != scan[1])
		{
			continue;
		}

		// scan[2] and match[2] are equal when the hash keys are
		scan += 2, match++;

		// insufficient lookahead is checked only every 8th comparison
		do
		{
		} while (*++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match &&
			*++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match &&
			scan < strend);

		len = MAX_MATCH_2CB8C0 - (int)(strend - scan);
		scan = strend - MAX_MATCH_2CB8C0;

		if (len > best_len)
		{
			s->match_start = cur_match;
			best_len = len;
			if (len >= nice_match)
			{
				break;
			}
			scan_end1 = scan[best_len - 1];
			scan_end = scan[best_len];
		}
	} while ((cur_match = prev[cur_match & wmask]) > limit && --chain_length != 0);

	if ((unsigned int)best_len <= s->lookahead)
	{
		return (unsigned int)best_len;
	}
	return s->lookahead;
}

// fills the window when the lookahead becomes insufficient, sliding it
// (and the hash chains) down when it is nearly full
// @retail 0x2cbf00
void __cdecl fill_window_2cbf00(deflate_state_2cb640 *s)
{
	unsigned int n, m;
	unsigned short *p;
	unsigned int more; // amount of free space at the end of the window
	unsigned int wsize = s->w_size;

	do
	{
		more = (unsigned int)(s->window_size - (unsigned long)s->lookahead - (unsigned long)s->strstart);

		if (more == 0 && s->strstart == 0 && s->lookahead == 0)
		{
			more = wsize;
		}
		else if (more == (unsigned int)(-1))
		{
			more--;
		}
		else if (s->strstart >= wsize + MAX_DIST_2CB8C0(s))
		{
			// the window is almost full: move the upper half to the lower
			memcpy(s->window, s->window + wsize, (unsigned int)wsize);
			s->match_start -= wsize;
			s->strstart -= wsize; // we now have strstart >= MAX_DIST
			s->block_start -= (long)wsize;

			// slide the hash table
			n = s->hash_size;
			p = &s->head[n];
			do
			{
				m = *--p;
				*p = (unsigned short)(m >= wsize ? m - wsize : NIL_2CB8C0);
			} while (--n);

			n = wsize;
			p = &s->prev[n];
			do
			{
				m = *--p;
				*p = (unsigned short)(m >= wsize ? m - wsize : NIL_2CB8C0);
			} while (--n);
			more += wsize;
		}
		if (s->strm->avail_in == 0)
		{
			return;
		}

		n = (unsigned int)read_buf_2cbc90(s->strm, s->window + s->strstart + s->lookahead, more);
		s->lookahead += n;

		// initialize the hash value now that we have some input
		if (s->lookahead >= MIN_MATCH_2CB8C0)
		{
			s->ins_h = s->window[s->strstart];
			UPDATE_HASH_2CB8C0(s, s->ins_h, s->window[s->strstart + 1]);
		}
	} while (s->lookahead < MIN_LOOKAHEAD_2CB8C0 && s->strm->avail_in != 0);
}

// flushes the current block, with given end-of-file flag
#define FLUSH_BLOCK_ONLY_2CB8C0(s, eof) \
	{ \
		tr_flush_block_2cdf70(s, \
			(s->block_start >= 0L ? (char *)&s->window[(unsigned int)s->block_start] : (char *)0), \
			(unsigned long)((long)s->strstart - s->block_start), (eof)); \
		s->block_start = (long)s->strstart; \
		flush_pending_2cb8f0(s->strm); \
	}

// same but returns from the block function if the output is full
#define FLUSH_BLOCK_2CB8C0(s, eof) \
	{ \
		FLUSH_BLOCK_ONLY_2CB8C0(s, eof); \
		if (s->strm->avail_out == 0) \
		{ \
			return (eof) ? finish_started_2cb8c0 : need_more_2cb8c0; \
		} \
	}

// copies without compression as much as possible from the input stream
// (level 0)
// @retail 0x2cc020
block_state_2cb8c0 __stdcall deflate_stored_2cc020(deflate_state_2cb640 *s, int flush)
{
	// stored blocks are limited to 0xffff bytes, pending_buf to
	// pending_buf_size, and each stored block has a 5 byte header
	unsigned long max_block_size = 0xffff;
	unsigned long max_start;

	if (max_block_size > s->pending_buf_size - 5)
	{
		max_block_size = s->pending_buf_size - 5;
	}

	// copy as much as possible from input to output
	for (;;)
	{
		// fill the window as much as possible
		if (s->lookahead <= 1)
		{
			fill_window_2cbf00(s);
			if (s->lookahead == 0 && flush == Z_NO_FLUSH_2CB8C0)
			{
				return need_more_2cb8c0;
			}
			if (s->lookahead == 0)
			{
				break; // flush the current block
			}
		}
		s->strstart += s->lookahead;
		s->lookahead = 0;

		// emit a stored block if pending_buf will be full
		max_start = (unsigned long)s->block_start + max_block_size;
		if (s->strstart == 0 || (unsigned long)s->strstart >= max_start)
		{
			// strstart == 0 is possible when wraparound on 16-bit machine
			s->lookahead = (unsigned int)(s->strstart - max_start);
			s->strstart = (unsigned int)max_start;
			FLUSH_BLOCK_2CB8C0(s, 0);
		}
		// flush if we may have to slide, otherwise block_start may become
		// negative and the data will be gone
		if (s->strstart - (unsigned int)s->block_start >= MAX_DIST_2CB8C0(s))
		{
			FLUSH_BLOCK_2CB8C0(s, 0);
		}
	}
	FLUSH_BLOCK_2CB8C0(s, flush == Z_FINISH_2CB8C0);
	return flush == Z_FINISH_2CB8C0 ? finish_done_2cb8c0 : block_done_2cb8c0;
}

// compresses as much as possible from the input stream without lazy
// evaluation of matches (levels 1-3)
// @retail 0x2cc170
block_state_2cb8c0 __stdcall deflate_fast_2cc170(deflate_state_2cb640 *s, int flush)
{
	unsigned int hash_head = NIL_2CB8C0; // head of the hash chain
	int bflush;                          // set if current block must be flushed

	for (;;)
	{
		// make sure there is always enough lookahead, except at the end of
		// the input
		if (s->lookahead < MIN_LOOKAHEAD_2CB8C0)
		{
			fill_window_2cbf00(s);
			if (s->lookahead < MIN_LOOKAHEAD_2CB8C0 && flush == Z_NO_FLUSH_2CB8C0)
			{
				return need_more_2cb8c0;
			}
			if (s->lookahead == 0)
			{
				break; // flush the current block
			}
		}

		// insert the string window[strstart .. strstart+2] in the
		// dictionary, and set hash_head to the head of the hash chain
		if (s->lookahead >= MIN_MATCH_2CB8C0)
		{
			INSERT_STRING_2CB8C0(s, s->strstart, hash_head);
		}

		// find the longest match, discarding those <= prev_length
		if (hash_head != NIL_2CB8C0 && s->strstart - hash_head <= MAX_DIST_2CB8C0(s))
		{
			if (s->strategy != Z_HUFFMAN_ONLY_2CB8C0)
			{
				s->match_length = longest_match_2cbd90(s, hash_head);
			}
		}
		if (s->match_length >= MIN_MATCH_2CB8C0)
		{
			tr_tally_dist_2cb8c0(s, s->strstart - s->match_start, s->match_length - MIN_MATCH_2CB8C0, bflush);

			s->lookahead -= s->match_length;

			// insert new strings in the hash table only if the match length
			// is not too large
			if (s->match_length <= s->max_lazy_match && s->lookahead >= MIN_MATCH_2CB8C0)
			{
				s->match_length--; // string at strstart already in hash table
				do
				{
					s->strstart++;
					INSERT_STRING_2CB8C0(s, s->strstart, hash_head);
				} while (--s->match_length != 0);
				s->strstart++;
			}
			else
			{
				s->strstart += s->match_length;
				s->match_length = 0;
				s->ins_h = s->window[s->strstart];
				UPDATE_HASH_2CB8C0(s, s->ins_h, s->window[s->strstart + 1]);
			}
		}
		else
		{
			// no match, output a literal byte
			tr_tally_lit_2cb8c0(s, s->window[s->strstart], bflush);
			s->lookahead--;
			s->strstart++;
		}
		if (bflush)
		{
			FLUSH_BLOCK_2CB8C0(s, 0);
		}
	}
	FLUSH_BLOCK_2CB8C0(s, flush == Z_FINISH_2CB8C0);
	return flush == Z_FINISH_2CB8C0 ? finish_done_2cb8c0 : block_done_2cb8c0;
}

// same as deflate_fast, but with lazy evaluation of matches: a match is
// emitted only if there is no better match at the next window position
// (levels 4-9)
// @retail 0x2cc470
block_state_2cb8c0 __stdcall deflate_slow_2cc470(deflate_state_2cb640 *s, int flush)
{
	unsigned int hash_head = NIL_2CB8C0; // head of hash chain
	int bflush;                          // set if current block must be flushed

	// process the input block
	for (;;)
	{
		if (s->lookahead < MIN_LOOKAHEAD_2CB8C0)
		{
			fill_window_2cbf00(s);
			if (s->lookahead < MIN_LOOKAHEAD_2CB8C0 && flush == Z_NO_FLUSH_2CB8C0)
			{
				return need_more_2cb8c0;
			}
			if (s->lookahead == 0)
			{
				break; // flush the current block
			}
		}

		if (s->lookahead >= MIN_MATCH_2CB8C0)
		{
			INSERT_STRING_2CB8C0(s, s->strstart, hash_head);
		}

		// find the longest match, discarding those <= prev_length
		s->prev_length = s->match_length, s->prev_match = s->match_start;
		s->match_length = MIN_MATCH_2CB8C0 - 1;

		if (hash_head != NIL_2CB8C0 && s->prev_length < s->max_lazy_match &&
			s->strstart - hash_head <= MAX_DIST_2CB8C0(s))
		{
			if (s->strategy != Z_HUFFMAN_ONLY_2CB8C0)
			{
				s->match_length = longest_match_2cbd90(s, hash_head);
			}
			if (s->match_length <= 5 &&
				(s->strategy == Z_FILTERED_2CB8C0 ||
					(s->match_length == MIN_MATCH_2CB8C0 && s->strstart - s->match_start > TOO_FAR_2CB8C0)))
			{
				// prev_match may be MIN_MATCH too: match_start is garbage,
				// but it will be ignored
				s->match_length = MIN_MATCH_2CB8C0 - 1;
			}
		}
		// if there was a match at the previous step and the current match
		// is not better, output the previous match
		if (s->prev_length >= MIN_MATCH_2CB8C0 && s->match_length <= s->prev_length)
		{
			unsigned int max_insert = s->strstart + s->lookahead - MIN_MATCH_2CB8C0;

			tr_tally_dist_2cb8c0(s, s->strstart - 1 - s->prev_match, s->prev_length - MIN_MATCH_2CB8C0, bflush);

			// insert in the hash table all strings up to the end of the
			// match (strstart-1 and strstart are already inserted)
			s->lookahead -= s->prev_length - 1;
			s->prev_length -= 2;
			do
			{
				if (++s->strstart <= max_insert)
				{
					INSERT_STRING_2CB8C0(s, s->strstart, hash_head);
				}
			} while (--s->prev_length != 0);
			s->match_available = 0;
			s->match_length = MIN_MATCH_2CB8C0 - 1;
			s->strstart++;

			if (bflush)
			{
				FLUSH_BLOCK_2CB8C0(s, 0);
			}
		}
		else if (s->match_available)
		{
			// no match at the previous position: output a single literal,
			// and flush the block if it was full
			tr_tally_lit_2cb8c0(s, s->window[s->strstart - 1], bflush);
			if (bflush)
			{
				FLUSH_BLOCK_ONLY_2CB8C0(s, 0);
			}
			s->strstart++;
			s->lookahead--;
			if (s->strm->avail_out == 0)
			{
				return need_more_2cb8c0;
			}
		}
		else
		{
			// no previous match to compare with: wait for the next step
			s->match_available = 1;
			s->strstart++;
			s->lookahead--;
		}
	}
	if (s->match_available)
	{
		tr_tally_lit_2cb8c0(s, s->window[s->strstart - 1], bflush);
		s->match_available = 0;
	}
	FLUSH_BLOCK_2CB8C0(s, flush == Z_FINISH_2CB8C0);
	return flush == Z_FINISH_2CB8C0 ? finish_done_2cb8c0 : block_done_2cb8c0;
}
