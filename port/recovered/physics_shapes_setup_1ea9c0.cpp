// @flags /O2 /Gr
/* the Havok shapes of a physics model tag (retail 0x1ea9c0, the tag index in
   eax and a byte on the stack): every shape block's elements carry an
   in-place shape object whose constructor retail inlines: the vtable at +0,
   0x80 at +6 (1 for the restored 0x455ef0 shapes) and at +8 the element that
   defines it (0 when restoring). Unless restoring, the references between
   shapes (a type in the low half, an index in the high half) are first
   turned into the shapes' addresses (0x1eaf30), and the list and multi-sphere
   shapes take their child and data arrays from the tag's shared blocks.

   The shape classes have no complete interfaces in src (see the draft parked
   in src/unknown_1ea960.cpp), so this writes the constructors' fields as
   retail does. The list shape's child count is its vtable's slot 10, fixed
   to 0x2fc1a0 (returns +0x10) since the vtable was just written. */

#include "unknown_11c920.h"
#include "globals.h"

struct s_shape_block_table;
void *function_1eaf30(short type, s_shape_block_table *table, short index);

enum
{
	k_vtable_4554c0 = 0x4554c0, /* the 0x80-byte elements' shape at +0x20 */
	k_vtable_455498 = 0x455498, /* the convex transform-like wrapper (+0xc: its shape) */
	k_vtable_456088 = 0x456088,
	k_vtable_455500 = 0x455500,
	k_vtable_455ef0 = 0x455ef0,
	k_vtable_456048 = 0x456048,
	k_vtable_456008 = 0x456008,
	k_vtable_455fc0 = 0x455fc0, /* list shape */
	k_vtable_455f58 = 0x455f58,
	k_vtable_455f30 = 0x455f30,
	k_vtable_455f8c = 0x455f8c,
};

static inline void shape_construct(byte *shape, dword vtable, dword owner, word flags = 0x80)
{
	*(dword *)(shape + 0) = vtable;
	*(word *)(shape + 6) = flags;
	*(dword *)(shape + 8) = owner;
}

static inline long block_count(byte *definition, long offset)
{
	return *(long *)(definition + offset);
}

static inline byte *block_address(byte *definition, long offset)
{
	return *(byte **)(definition + offset + 4);
}

static inline void *resolve_reference(long key, byte *definition)
{
	return function_1eaf30((short)key, (s_shape_block_table *)definition, (short)(key >> 16));
}

// @retail 0x1ea9c0
void function_1ea9c0(long tag_index, long restore_argument)
{
	byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
	bool restore = (byte)restore_argument != 0;

	if (!restore)
	{
		for (long i = 0; i < block_count(definition, 0x38); ++i)
		{
			byte *entry = block_address(definition, 0x38) + i * 0x90;
			*(void **)(entry + 0x38) = resolve_reference(*(long *)(entry + 0x38), definition);
		}
	}

	for (long i = 0; i < block_count(definition, 0x48); ++i)
	{
		byte *entry = block_address(definition, 0x48) + i * 0x80;
		shape_construct(entry + 0x20, k_vtable_4554c0, restore ? 0 : (dword)entry);
		if (!restore)
			*(dword *)(entry + 0x30 + 0xc) = (dword)(entry + 0x20);
		shape_construct(entry + 0x30, k_vtable_455498, 0);
	}

	for (long i = 0; i < block_count(definition, 0x50); ++i)
	{
		byte *entry = block_address(definition, 0x50) + i * 0xb0;
		shape_construct(entry + 0x20, k_vtable_456088, restore ? 0 : (dword)entry);
	}

	for (long i = 0; i < block_count(definition, 0x58); ++i)
	{
		byte *entry = block_address(definition, 0x58) + i * 0x50;
		shape_construct(entry + 0x20, k_vtable_455500, restore ? 0 : (dword)entry);
	}

	for (long i = 0; i < block_count(definition, 0x60); ++i)
	{
		byte *entry = block_address(definition, 0x60) + i * 0x90;
		shape_construct(entry + 0x20, k_vtable_455ef0, restore ? 0 : (dword)entry, restore ? 1 : 0x80);
		if (!restore)
			*(dword *)(entry + 0x40 + 0xc) = (dword)(entry + 0x20);
		shape_construct(entry + 0x40, k_vtable_455498, 0);
	}

	for (long i = 0; i < block_count(definition, 0x68); ++i)
	{
		byte *entry = block_address(definition, 0x68) + i * 0x60;
		shape_construct(entry + 0x20, k_vtable_456048, restore ? 0 : (dword)entry);
	}

	/* the multi-sphere-like shapes: more than three elements live in the
	   shared block at +0x78, the second array always in the block at +0x80 */
	long first_count = 0;
	long second_count = 0;
	for (long i = 0; i < block_count(definition, 0x70); ++i)
	{
		byte *entry = block_address(definition, 0x70) + i * 0x100;
		byte *shape = entry + 0x20;
		if (restore)
		{
			shape_construct(shape, k_vtable_456008, 0);
			continue;
		}
		bool inline_first = (*(dword *)(shape + 0x38) & 0x7fffffff) <= 3;
		byte *first = inline_first ? shape + 0x40 : block_address(definition, 0x78) + first_count * 0x30;
		byte *second = block_address(definition, 0x80) + second_count * 0x10;
		*(dword *)(shape + 0x30) = (dword)first;
		*(dword *)(shape + 0xd4) = (dword)second;
		shape_construct(shape, k_vtable_456008, (dword)entry);
		first_count += inline_first ? 0 : *(long *)(shape + 0x34);
		second_count += *(long *)(shape + 0xd8);
	}

	/* the list shapes: more than four children live in the shared block at
	   +0x98; each child reference becomes its shape */
	long child_total = 0;
	for (long i = 0; i < block_count(definition, 0x90); ++i)
	{
		byte *entry = block_address(definition, 0x90) + i * 0x38;
		if (restore)
		{
			shape_construct(entry, k_vtable_455fc0, 0);
			continue;
		}
		bool inline_children = (*(dword *)(entry + 0x14) & 0x7fffffff) <= 4;
		byte *children = inline_children ? entry + 0x18 : block_address(definition, 0x98) + child_total * 8;
		*(dword *)(entry + 0xc) = (dword)children;
		shape_construct(entry, k_vtable_455fc0, 0);
		for (long j = 0; j < *(long *)(entry + 0x10); ++j)
		{
			*(void **)(children + j * 8) = resolve_reference(*(long *)(children + j * 8), definition);
			*(long *)(children + j * 8 + 4) = 0;
		}
		child_total += inline_children ? 0 : *(long *)(entry + 0x10);
	}

	/* shapes over a list shape (the high half of +0xc) with their data from
	   the block at +0xa8, each 16-byte aligned */
	long data_offset = 0;
	for (long i = 0; i < block_count(definition, 0xa0); ++i)
	{
		byte *entry = block_address(definition, 0xa0) + i * 0x14;
		if (restore)
		{
			shape_construct(entry, k_vtable_455f58, 0);
			continue;
		}
		long key = *(long *)(entry + 0xc);
		byte *child = block_address(definition, 0x90) + (key >> 16) * 0x38;
		byte *data = block_address(definition, 0xa8) + data_offset;
		*(dword *)(entry + 0x10) = (dword)data;
		*(dword *)(entry + 0xc) = (dword)child;
		shape_construct(entry, k_vtable_455f58, 0);
		data_offset += (*(long *)(data + 0x20) + 15) & ~15;
	}

	/* shapes with a child reference and a second object at +0x14 that keeps
	   the value at +0x1c */
	for (long i = 0; i < block_count(definition, 0x108); ++i)
	{
		byte *entry = block_address(definition, 0x108) + i * 0x20;
		byte *second = entry + 0x14;
		if (restore)
		{
			shape_construct(entry, k_vtable_455f30, 0);
			shape_construct(second, k_vtable_455f8c, 0);
			continue;
		}
		dword saved = *(dword *)(second + 8);
		*(void **)(entry + 0xc) = resolve_reference(*(long *)(entry + 0xc), definition);
		*(dword *)(entry + 0x10) = (dword)second;
		shape_construct(entry, k_vtable_455f30, 0);
		shape_construct(second, k_vtable_455f8c, saved);
	}
}
