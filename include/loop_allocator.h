/* LOOP_ALLOCATOR.H: types shared by the memory, voice and callback-table sources and stubs, the
   callbacks and vtable at 0x453c00 */

#ifndef LOOP_ALLOCATOR_H
#define LOOP_ALLOCATOR_H

#include "screen_widgets.h"
#include "unknown_11c920.h"

/* the memory source of a loop allocator, and the allocators at 0x476fbc and
   0x47d924 (objects whose first dword is the vtable) */
class c_memory_source
{
public:
	virtual void *allocate(long size) { return 0; }
	virtual void release(void *block) {}
};

/* a block of a loop allocator: its size (header included), the pointer
   that owns it, and its neighbours; with debug headers, a 'head' header
   (file, line, time) comes first */
struct s_loop_block
{
	long size;
	void **owner;
	s_loop_block *next;
	s_loop_block *previous;
};

struct s_loop_block_debug_header
{
	dword signature;
	char const *file;
	long line;
	dword time;
};

/* a "loop" allocator (0x40 byte header, constructed by 0x18e250; its pool
   follows, aligned to 16 bytes, so the sources allocate 0x50 more bytes) */
struct s_loop_allocator
{
	dword signature;
	char name[0x20];
	c_memory_source *source;
	byte *base;
	long size;
	long free;
	s_loop_block *first;
	s_loop_block *last;
	byte field3c;
	byte field3d;
	byte field3e;
	bool debug_headers;
};

s_loop_allocator *function_18e1f0(long size, c_memory_source *source, const char *name);
void function_18e250(s_loop_allocator *loop, long size, const char *name, c_memory_source *source);
void function_18e230(s_loop_allocator *loop);
bool function_18eea0(long stage);

/* the class of the vtable at 0x453c48 (slot 0 is the deleting destructor at
   0x18f3d4), derived from a base whose destructor is 0x18f3f0; the slots
   after 0x24a01f are not decompiled yet */
/* the list's item widgets (screen_widgets.h) */
typedef c_class_14750b c_unknown_249fa3_entry;

class c_list_24a64d : public c_list_widget_with_items
{
public:
	c_list_24a64d(word arg_0);
	virtual void v20(c_class_1a2c81 *item, long);
	virtual void *function_24a64d(long index);
	virtual void v24();
	virtual word const *function_24a691(long index, bool append);
	bool function_24a2e7(long controller);
	void function_24a30e(s_controller_reference **arg_0, long *arg_1);
	union { byte field_88; byte field88; };
	union { bool field_89; byte field89; };
	union { bool field_8a; bool field8a; };
	byte unknown8b;
	union { long field_8c; long field8c; };
	c_list_item_handler field_90;
};

typedef c_list_24a64d c_unknown_249fa3_base;

struct s_profile_list_entry
{
	s_profile_list_entry();
	long profile_index;
	s_player_profile_settings settings;
	byte unknown1e4[0x248 - 0x1e4];
};

class c_unknown_249fa3 : public c_unknown_249fa3_base
{
public:
	c_unknown_249fa3(word user_flags);
	virtual void v1();
	virtual void v2();
	virtual void v3();
	virtual void *get_item_data();
	virtual void *get_items(long *count);

	c_unknown_249fa3_entry entries[4];
	s_profile_list_entry profiles[4];
};

class c_profile_mode_screen : public c_screen_with_menu
{
public:
	c_profile_mode_screen(long a, long b, word user_flags);
	virtual void v17();
	virtual screen_load_proc get_load_proc();
	c_unknown_249fa3 menu;
	/* choosing a profile also signs it in online */
	bool sign_in_online;
};

#endif
