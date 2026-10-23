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

// Disabled: matches alone, but closes the call cycle b8540 -> 109400 -> e4c10 -> e18d0 -> b75a0 and costs matched d6bc0, 1c4260 and the physics component functions (build/codex-report.md, round 25).
#if H2_NATIVE_PORT
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
