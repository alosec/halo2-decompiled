// @flags /O2 /Ob1 /Gr /arch:SSE
#include "unknown_1cec30.h"

struct s_278f00
{
	byte field_0[0x3c];
	hkRigidBody **field_3c;
	long field_40;
	byte field_44[0x6a];
	bool field_ae;
};

struct s_278f01
{
	s_278f00 **field_0;
	long field_4;
	long field_8;
};

struct s_278f02
{
	byte field_0[3];
	byte field_3;
	byte field_4[4];
	s_havok_object *field_8;
};

struct s_278f03
{
	byte field_0[0xc0];
	word : 5;
	word field_c0_5 : 1;
	word field_c0 : 1;
	word : 9;
	byte field_c2[0x12];
	long field_d4;
};

extern hkWorld *g_51e9a4;
extern bool g_55e4fc;
extern bool g_47f05b;
void __stdcall function_1c3770(long arg_0, dword arg_1);
void function_146bf0();
void function_1d1260(s_havok_component *arg_0);
void __stdcall function_b8540(long arg_0);
void __stdcall function_b8890(long arg_0);

bool function_a7670(long object_index);

// score 181 (was 212): frame 0x424 and retail's length agree; left are the scratch-register phase at entry
// (retail xor eax), the homes of the list/island/index/count slots and the inner-loop induction, the
// admission flag (retail BL, ours a stack byte) and the cursor cleanup loop (retail indexed).
// list reference 212; island reference 212.
// body reference 212; header reference 212.
// unsigned object handle 212; list and island references 212.
// first standard search 244; second standard search 282.
// existing component-delete helper 341; existing detach helper 464; inline declarations
// 341/464; category byte comparisons 266; signed-byte category 212, membership 212, both 212.
// Outer selector byte/short/word/dword 219/266/266/212; byte/short with branched
// selection 245/291; byte/short with indexed cleanup 226/269. Original restored.
// Distinct first/second membership ordinals: byte/long, short/long, word/long 281 each;
// long/byte 278; long/short and long/word 267; byte/byte 289; short/short 290.
// Round 225: First/second array list/island ownership pairs 01/02/10/11/12/20/21/22: 212 each.
// Round 227: Remove redundant first/second/both pre-retry initializations 212/212/212; split
// first/second/both insertion increments 212/212/212; both removals with second/both split increments
// 212/212. Original resets and body restored.
// Compound-termination first/second index 266/212; both indices 266; first/second
// begin-end cursors 291/279; both cursors 319; index/cursor 271; cursor/index 291.
// Round 330: Full-width unsigned first/second/search domains, seven combinations 212 each; all three with
// both processing ordinals 212. Signed body/list bounds, retry and call cycle retained; original restored.
// Round 178: Reprocess islands with explicit advance on unflagged slots, for-loop selector/membership
// combinations 270/270/241/239; while-loop combinations 270/270/241/239. All reach 954 bytes but regress;
// original induction restored. List/index/second-count homes and cleanup scheduling remain.
// Round 235: Force-inline declaration at component-delete/detach boundaries 341/464;
// with cleanup helper 411/532; with branched list selection 350/469; both boundaries
// with branched selection 404/538. All helpers stay out of line; original body restored.
// Complementary category bool/byte/short/long with signed second count 212/212/271/273;
// unsigned second-count counterparts 212/212/271/273. Wide category frame 0x428; all restored.
// Round 194: Shared array/header/group alone, all three pairs and all three together 212 each; all native
// records with branched list selection 244. Original body and views restored.
// Unsigned first/second/both handle arrays 212/212/212; first/second/both
// arrays with unsigned classification 212/212/212; complete unsigned processing 212;
// unsigned arrays/processing with original signed classification 212. Original storage restored.
// Round 246: category after first/both membership scans with early header 252/291;
// move header too 306/302; indexed-cleanup counterparts 257/296/312/311.
// Native byte flag unit with unchanged/bit-5/bit-6/both writes 212 each;
// signed-byte flag unit with those four write forms 255 each. Original restored.
// Lane 4 r98: list chosen per arm (global read in each, as retail) 244; descriptive local names give
// identical code (212); plain indexed cleanup loop (retail's inc/cmp count form) 217 (964 bytes); word
// bit-5/bit-6 fields in s_278f03 fuse both flag writes into retail's `or/and byte ptr` but 939 bytes, 253;
// a named bool for the mode-4 test folds away (212). Retail evaluates that test's second half as
// `setne/test` (as 0x1c4040 does): an inline bool helper shared with 0x1c4040 is untried.
// Round 224: First-list processing cursors, read-only/mutable and consumed/body-end steps,
// original/branched selection 283/221/283/221/272/266/272/266. Original indexed processing and call cycle restored.
// Header-byte-offset induction long/dword 276 each, short/word/byte 280 each;
// direct descending list selector long/short/signed-byte 251/265/252. Restored.
// Round 102: the mode-4 test through the existing 0xa7670 (inlined with retail's setne/test) 281 alone;
// with word bit-5/bit-6 fields written through them 250 (954 B); with the list chosen per arm 181,
// kept. On it: descriptive names 181; list declared at function scope 181; indexed cleanup loop 196;
// found flag declared before the admission flag 274; the inner bound read from the field 248.
// Round 63: list chosen by if/else arms (long selector) 244; ternary of the two list addresses 244;
// descriptive names for the outer locals 212; second scan with its own found flag 269 (flag materialized,
// not threaded); index comparison before the first-scan flag 213. Diagnostics, not kept: the predicted-copy
// test as a one-line inline bool helper 212, as a single-exit helper 281 (gives the setne/test).
// @retail 0x278f00
void function_278f00()
{
	long local_0[128];
	long local_1[128];
	if (g_55e4fc && !g_47f05b)
	{
		g_47f05b = true;
		for (long local_2 = 0; local_2 < 2; local_2++)
		{
			s_278f01 *local_3;
			if (local_2 == 0)
				local_3 = (s_278f01 *)((byte *)g_51e9a4 + 8);
			else
				local_3 = (s_278f01 *)((byte *)g_51e9a4 + 0x14);
			for (long local_4 = 0; local_4 < local_3->field_4; local_4++)
			{
				s_278f00 *local_5 = local_3->field_0[local_4];
				if (!local_5->field_ae)
					continue;
				local_4--;
				local_5->field_ae = false;
				long local_6 = local_5->field_40;
				long local_7 = 0;
				long local_8 = 0;
				long local_9 = 0;
				do
				{
					local_7 = 0;
					local_8 = 0;
					for (long local_10 = 0; local_10 < local_6; local_10++)
					{
						hkRigidBody *local_11 = local_5->field_3c[local_10];
						if (!local_11->m_fixed)
						{
							long local_12 = havok_component_get(havok_entity_property_get((hkEntity *)local_11, 0x2001))->object_index;
							s_278f02 *local_13 = &((s_278f02 *)g_4e0300->data)[local_12 & 0xffff];
							bool local_14 = local_9 > 0 || !((1 << local_13->field_3) & 0x81);
							bool local_15 = false;
							long local_16;
							for (local_16 = 0; local_16 < local_7; local_16++)
							{
								if (local_0[local_16] == local_12)
								{
									local_15 = true;
									break;
								}
							}
							for (local_16 = 0; local_16 < local_8; local_16++)
							{
								if (local_1[local_16] == local_12)
									break;
							}
							if (!local_15 && local_16 == local_8)
							{
								if (local_14)
									local_0[local_7++] = local_12;
								else
									local_1[local_8++] = local_12;
							}
						}
					}
					if (local_7 > 0)
						break;
					local_9++;
				} while (local_9 < 2);
				for (long local_10 = 0; local_10 < local_7; local_10++)
				{
					long local_11 = local_0[local_10];
					s_havok_object *local_12 = havok_object_get(local_11);
					if (function_a7670(local_11))
					{
						((s_278f03 *)local_12)->field_c0_5 = true;
						function_1c3770(local_11, 0);
					}
					else
					{
						bool local_13 = TEST_FIELD_BIT(((s_278f03 *)local_12)->field_c0);
						if (local_13)
							function_146bf0();
						local_12 = havok_object_get(local_11);
						long local_14 = local_12->havok_component_index;
						if (local_14 != NONE)
						{
							s_havok_component *local_15 = havok_component_get(local_14);
							long local_16 = local_15->object_index;
							local_15->~s_havok_component();
							record_pool_release(g_51e9b8, local_14);
							s_havok_object *local_17 = havok_object_get(local_16);
							if (*((byte *)local_17 + 0xc0) & 1)
							{
								local_17->havok_flag = false;
								(*g_51e9a0)--;
							}
							local_12->havok_component_index = NONE;
						}
						local_12 = havok_object_get(local_11);
						if (*((byte *)local_12 + 0xc0) & 1)
						{
							local_12->havok_flag = false;
							(*g_51e9a0)--;
						}
						if (local_13)
						{
							function_278f00();
							function_146bf0();
						}
						function_b8540(local_11);
					}
				}
				long const *cleanup_cursor = local_1;
				for (long local_10 = 0; local_10 < local_8; local_10++)
				{
					long local_11 = *cleanup_cursor++;
					s_havok_object *local_12 = havok_object_get(local_11);
					if (local_12->havok_component_index != NONE)
						function_1d1260(havok_component_get(local_12->havok_component_index));
					((s_278f03 *)local_12)->field_c0 = false;
					function_b8890(local_11);
				}
			}
		}
		g_47f05b = false;
		g_55e4fc = false;
	}
}
