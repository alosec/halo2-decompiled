// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0B8540.CPP: taking an object out of the world: it is marked
   hidden, detached from its parent's connections and its per-object state
   (attachments, scenery, sounds, collision) is released. */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_queries.h"

/* the four bytes after a long and a short at +0xa4 */
struct s_object_hide_identifier
{
	long field_0;
	short field_4;
	byte field_6;
	char field_7;
};

struct s_object_hide_view
{
	byte unknown00[0xa4];
	s_object_hide_identifier identifier;
	byte unknownac[0xaf - 0xac];
	char field_af;
};

struct s_object_hide_header
{
	byte unknown00[2];
	byte flags;
	byte type;
	byte unknown04[4];
	s_object_hide_view *object;
};

bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool own_flags);
void function_b8b70(long object_index);
void __stdcall function_10a250(long scenery_index);
void function_bf090(long object_index);
void function_15b220(long object_index, long index);
void __stdcall object_list_erase(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_a7a60(long index);
void object_interpolated_values_clear_all(long object_index);

static inline void object_identifier_clear(s_object_hide_identifier *identifier)
{
	identifier->field_7 = -1;
	identifier->field_4 = -1;
	identifier->field_0 = -1;
}

// Disabled: matches alone, but enabling it (stub removed) loses matched functions in a full check, because the live edge
// b8540 -> 109400 -> e4c10 -> e18d0 -> b75a0 lets LTCG enter the object code's big call cycle at b8540 (reached first from
// ai.cpp's ai_dispose_from_old_map through 0x292e00) and walk down 109400 -> e4c10, which changes which cycle member keeps its
// default convention. Measured on r509 (9722 matched):
//  - enabled alone: 23 lost (b6df0 b7290 b8600 b9a90 d6bc0 10a660 10ad40 10cd50 11a320 11b520 1c2b10 1c3770 1c37f0 1c3850
//    1c38a0 1c4260 1cec80 1cf220 1cf9f0 1d2460 1e54d0 1ed340 2a1800). Conventions that flip: 0x1c3770 (retail pushes both
//    arguments, ret 8; ours eax, ret 4), 0xd6800 (retail pushes four, ret 0x10; ours ecx/edx, so d6bc0 and 11a320 differ),
//    0x1ed340 (retail ecx; ours stack, so e4c10 differs once it is enabled) and the register choices of 1cec80, 1cf220
//    and 1cf9f0. Cutting only the 109400 -> e4c10 edge (a local probe) restores b7290, 1c3850, 1c38a0 and e4c10
//    and leaves d6bc0 at 3, so that edge is the main trigger.
//  - declaring function_e18d0 before function_1ed340 in bipeds.cpp (call order at e4c10 follows the TU's declaration order):
//    9 lost (d6bc0 11a320 1c2b10 1c37f0 1c38a0 1c4260 1cec80 1cf220 1cf9f0); e18d0 then keeps the stack convention.
//    Also declaring function_1c3770 before function_c5460: 7 lost (b8890 d6bc0 11a320 1c35f0 1c3770 1c38a0 1c39c0).
//  - tried and no better: b8540's file name earlier or later in link order (23 to 41 lost), dropping the 292e00 call (7).
//  - with the b7430, b7b40 and 2902b0 drafts enabled as well (b7b40's draft does not match), d6bc0 loses to the same
//    0xd6800 convention flip. In retail the walk enters this cycle through b75a0 -> b7430, not at b8540, so enabling b8540
//    cleanly probably needs those three functions (or a different entry into the cycle) first.
#if H2_NATIVE_PORT || H2_WHOLE_IMAGE   /* port: enabled in the native builds */
// @retail 0xb8540 disabled
void __stdcall function_b8540(long object_index)
{
	s_object_hide_header *header = (s_object_hide_header *)g_4e0300->data + (object_index & 0xffff);
	s_object_hide_view *object = header->object;

	if (!object_or_parent_hidden(object_index) && function_b9d20(object_index))
		function_bef30(object_index, 1, 0, 0, false);
	header->flags |= 0x10;
	function_b8b70(object_index);
	if ((1 << header->type) & 0x40)
		function_10a250(object_index);
	function_bf090(object_index);
	object_identifier_clear(&object->identifier);
	if (object->field_af != -1)
		function_15b220(object_index, object->field_af);
	object_list_erase(object_index);
	function_bb950(object_index, false, -1);
	function_a7a60(object_index);
	object_interpolated_values_clear_all(object_index);
}
#endif
