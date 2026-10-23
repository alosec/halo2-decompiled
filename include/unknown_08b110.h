/* UNKNOWN_08B110.H: the classes whose vtables are at 0x450c94..0x450d4c (batch 19-2).
   The table holds seven vtables: 0x450c94 (9 slots), 0x450cb8 (6), 0x450cd0 (9),
   0x450cf4 (8), 0x450d14 (2), 0x450d1c (9) and 0x450d40 (3). The ones at 0x450c94
   and 0x450d1c share the same interface (c_interface_450c94, which supplies slots 2 and 4). */

#ifndef UNKNOWN_08B110_H
#define UNKNOWN_08B110_H

#include <xtl.h>
#include "unknown_11c920.h"
#include "globals.h"
#include "bitstream.h"

/* ---- shared structures ---- */

/* a block of the interface: x04 is the index of the target, the data pointers
   start at x40 (two dwords per entry) */
struct s_block_450c94
{
	long unknown00;
	long index;
	byte unknown08[0x34];
	long count;
	void *data[16];
};

struct s_counter_450c94
{
	byte unknown00[0x10];
	long count;
};

/* a record in a message: 8 bytes (size, type, data) */
struct s_item_450cb8
{
	short size;
	short type;
	void *data;
};

/* ---- the interface of 0x450c94 and 0x450d1c ---- */

struct s_node_450d1c;

class c_interface_450c94
{
public:
	virtual bool v0() { return false; }
	virtual long v1(long a1, long max_count, void *entries) { return 0; }
	virtual long v2();
	virtual void v3(s_node_450d1c *node, long a2, long a3, long key, s_bitstream *stream, long reserved_bits) {}
	virtual void v4(long a1, s_counter_450c94 *a2);
	virtual long v5(dword a1, s_bitstream *stream, long max_blocks, s_block_450c94 *blocks, long *count) { return 0; }
	virtual void v6(s_block_450c94 *block) {}
	virtual void v7() {}
	virtual void v8() {}
};

/* the entries v1 of the 0x450c94 class writes (20 bytes) */
struct s_entry_450c94
{
	long unknown00;
	real unknown04;
	long size;
	long unknown0c;
	long index;
};

/* the object used to look up the three tunables per entry */
class c_source_450c94
{
public:
	virtual void v0() {}
	virtual void v1(long a1, long a2, real *a3, long *a4) {}
};

struct s_dword34
{
	dword v[13];
};

struct s_dword40
{
	dword v[16];
};

/* the 0x450c94 class */
class c_vtable_450c94 : public c_interface_450c94
{
public:
	c_vtable_450c94() : initialized(false) {}
	~c_vtable_450c94();
	virtual bool v0();
	virtual long v1(long a1, long max_count, void *entries);
	virtual void v3(s_node_450d1c *node, long a2, long a3, long key, s_bitstream *stream, long reserved_bits);
	virtual long v5(dword a1, s_bitstream *stream, long max_blocks, s_block_450c94 *blocks, long *count);
	virtual void v6(s_block_450c94 *block);

	void reset(c_source_450c94 *new_source);
	bool take_data18(long index, s_dword34 *data);
	void set_data720(long index, s_dword40 const *data);

	long unknown04;
	bool initialized;
	byte unknown09[3];
	c_source_450c94 *source;
	dword active_mask;
	dword mask14;
	s_dword34 data18[32];
	dword times[32];
	dword unknown718;
	dword mask71c;
	s_dword40 data720[32];
};

/* ---- the 0x450cb8 class: dispatches to a handler per message type ---- */

struct s_message_450cb8
{
	long unknown00;
	long type;
	byte unknown08[8];
	long handles[2];
};

struct s_slot_450cb8
{
	long handle;
	long unknown04;
	short state;
	byte unknown0a[10];
};

struct s_flag_450cb8
{
	byte flags;
	byte unknown01[7];
};

struct s_flags_450cb8
{
	byte unknown00[0x44];
	s_flag_450cb8 entries[0x400];
};

struct s_table_450cb8
{
	byte unknown00[0x14];
	s_flags_450cb8 *flags;
	byte unknown18[0xc];
	s_slot_450cb8 slots[0x400];
};

struct s_block_a_450cb8
{
	byte unknown00[0x30];
	s_table_450cb8 table;
};

struct s_world_a_450cb8
{
	byte unknown00[8];
	s_block_a_450cb8 *block;
};

struct s_world_450cb8
{
	byte unknown00[4];
	s_world_a_450cb8 *a;
};

struct s_context_450cb8
{
	s_world_450cb8 *world;
};

bool function_979f0(s_world_450cb8 *world, long handle);

class c_handler_450cb8
{
public:
	virtual long v0() { return 0; }
	virtual void v1() {}
	virtual long v2() { return 0; }
	virtual long v3() { return 0; }
	virtual bool v4() { return false; }
	virtual bool v5(s_message_450cb8 *a1, s_context_450cb8 *a2) { return false; }
	virtual void v6(s_message_450cb8 *a1, s_context_450cb8 *a2, long *a3) {}
	virtual real v7(s_message_450cb8 *a1, s_context_450cb8 *a2, long a3) { return 0.0f; }
	virtual void v8(s_message_450cb8 *a1, long a2, long a3, long a4, long a5) {}
	virtual void v9(long a1, long a2, long a3) {}
	virtual bool v10(long a1, void *a2, void *a3) { return false; }
	virtual void v11(long a1, long a2, long a3, void *a4) {}
};

struct s_handlers_450cb8
{
	byte unknown00[0x84];
	long count;
	c_handler_450cb8 *handlers[1];
};

struct s_datum_450cb8
{
	long handle;
	byte unknown04[3];
	byte counter;
	byte unknown08[0x18];
};

struct s_datums_450cb8
{
	byte unknown00[0x14];
	s_datum_450cb8 data[0x400];
};

inline s_datum_450cb8 *datum_try_get_450cb8(s_datums_450cb8 *datums, long handle)
{
	return datums->data[handle & 0x3ff].handle == handle ? &datums->data[handle & 0x3ff] : 0;
}

class c_vtable_450cb8
{
public:
	virtual long v0(long index, long a2, long a3, long *count, s_item_450cb8 *items, void *a6);
	virtual void v1(long index, long a2, long a3, s_item_450cb8 *item);
	virtual void v2(long index, long a2, long a3, long a4);
	virtual void v3(s_message_450cb8 *a1);
	virtual void v4(s_message_450cb8 *a1, s_context_450cb8 *a2, real *a3, long *a4);
	virtual void v5(s_message_450cb8 *a1, long a2, long a3, long a4);

	byte unknown04[0xc];
	s_handlers_450cb8 *handlers;
	s_datums_450cb8 *datums;
};

/* g_4ced60: a table of 20-byte definitions indexed by the handler's v0 */
struct s_definition_4ced60
{
	real unknown00;
	long unknown04;
	long unknown08;
	real unknown0c;
	real unknown10;
};

extern s_definition_4ced60 g_4ced60[];

/* a global tunable of 0x450c94 and the time source */
extern long g_4cf474;


inline long time_now()
{
	if (g_510548)
		return g_51054c;
	return GetTickCount();
}

void *function_96e90(long size);

/* ---- the bit stream (0x1959c0 reads, 0x195720 writes) ---- */


/* ---- the 0x450d1c class: a sibling of 0x450c94 that serves queued nodes ---- */

struct s_entry_450d1c;

struct s_node_450d1c
{
	long unknown00;
	long unknown04;
	long time;
	long timeout;
	long unknown10;
	long unknown14;
	void *data;
	long size;
	dword active_mask;
	dword done_mask;
	s_node_450d1c *next;
};

struct s_entry_450d1c
{
	long unknown00;
	real unknown04;
	long size;
	long unknown0c;
	s_node_450d1c *node;
};

class c_manager_450d1c
{
public:
	virtual long v0(long type, dword *items, long count, long *produced, dword *data, s_bitstream *stream) { return 0; }
	virtual void v1(long type, dword *items, long count, dword *data) {}
	virtual void v2(long type, long size, void const *data, s_bitstream *stream) {}
	virtual void v3(s_node_450d1c *node) {}
	virtual void v4(s_node_450d1c *node, long a1, real *a2, long *a3) {}
};

struct s_owner_450d1c
{
	byte unknown00[4];
	c_manager_450d1c *manager;
	byte unknown08[0x40];
	long count;
	s_node_450d1c *head;
};

void function_89e70(s_owner_450d1c *owner, s_node_450d1c *node);
s_node_450d1c *function_89eb0(s_node_450d1c *node, long flags);
class c_vtable_450d1c;

/* a pending request: the nodes it waits on hang off x04 */
struct s_link_450d1c
{
	s_node_450d1c *node;
	s_link_450d1c *next;
};

struct s_request_450d1c
{
	long key;
	s_link_450d1c *links;
	s_request_450d1c *next;
};

class c_vtable_450d1c : public c_interface_450c94
{
public:
	c_vtable_450d1c() : unknown08(0) {}
	~c_vtable_450d1c();
	virtual bool v0();
	virtual long v1(long a1, long max_count, void *entries);
	virtual void v3(s_node_450d1c *node, long a2, long a3, long key, s_bitstream *stream, long reserved_bits);
	virtual long v5(dword a1, s_bitstream *stream, long max_blocks, s_block_450c94 *blocks, long *count);
	virtual void v6(s_block_450c94 *block);
	virtual void v7(void *a1);
	virtual void v8(long a1, bool a2);

	void function_976f0(long key, bool flag);

	long unknown04;
	byte unknown08;
	byte unknown09;
	byte unknown0a[2];
	long player;
	s_request_450d1c *requests;
	long request_count;
	s_owner_450d1c *owner;
	long unknown1c;
	long pending;
	long unknown24;
};

/* ---- the 0x450d40 base and the 0x450cf4 class (an aggregate of three children) ---- */

class c_base_450d40
{
public:
	virtual void v0() {}
	virtual void v1() {}
	virtual bool v2(bool *a1) { return false; }
};

class c_vtable_450cf4 : public c_base_450d40
{
public:
	c_vtable_450cf4() { unknown04[0] = 0; }
	~c_vtable_450cf4();
	virtual bool v2(bool *a1);
	virtual long v3(long a1, long a2);
	virtual void v4() {}
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7() {}

	byte unknown04[8];
	c_interface_450c94 *children[3];
	byte unknown18[0xc];
	long unknown24;
};

/* ---- the 0x450d14 class ---- */

struct s_vector_450d14
{
	real v[3];
};

struct s_entry_450d14
{
	long unknown00;
	long unknown04;
	s_vector_450d14 unknown08;
	s_vector_450d14 unknown14;
	byte unknown20;
	byte unknown21[3];
};

struct s_key_450d14
{
	word v[3];
};

struct s_object_450d14
{
	byte unknown00[0x64];
	s_vector_450d14 unknown64;
	byte unknown70[0xd4 - 0x70];
	long unknownd4;
	byte unknownd8[0x15c - 0xd8];
	s_vector_450d14 unknown15c;
	byte unknown168[0x241 - 0x168];
	byte unknown241;
};

struct s_player_450d14
{
	byte unknown00[0x1a];
	short unknown1a;
	short unknown1c;
	byte unknown1e[0x2c - 0x1e];
	long unknown2c;
	long unknown30;
	byte unknown34[0x21c - 0x34];
};

struct s_header_450d14
{
	byte unknown00[8];
	s_object_450d14 *object;
};

struct s_world_450d14
{
	byte unknown00[2];
	byte kind;
	byte unknown03[9];
	void *unknown0c;
	byte unknown10[4];
	s_key_450d14 key;
	byte unknown1a[0x78 - 0x1a];
	byte unknown78;
	byte unknown79[3];
	long unknown7c;
};

class c_vtable_450d14;

struct s_sub_450d14
{
	c_vtable_450d14 *owner;
	byte flag;
	byte unknown05[3];
	long value;
	long count;
	s_entry_450d14 entries[4];
};

class c_vtable_450d14
{
public:
	virtual s_sub_450d14 *v0();
	virtual void v1(long a1, long a2, real *a3, long *a4);

	s_world_450d14 *world;
	s_sub_450d14 sub;
};

struct s_match_450d14
{
	long unknown00;
	long unknown04;
};

s_match_450d14 *__stdcall function_6a3b0(void *table, s_key_450d14 *key, long index);
long function_699a0(long index, void *table);
void __stdcall function_82a40(long object_index, struct s_relevance_observers const *observers, real *relevance, long *value);



/* ---- the 0x450cd0 class: the handle table of unknown_096ed0.h ---- */

class c_handle_table_450cd0;
struct s_bitstream;
bool function_98620(c_handle_table_450cd0 *self, s_bitstream *stream, long index, long a3, long reserved_bits);
bool function_986d0(c_handle_table_450cd0 *self, long index, s_bitstream *stream, long reserved_bits);
bool function_98750(c_handle_table_450cd0 *self, s_bitstream *stream, long index, long a3, long reserved_bits);
bool function_988f0(c_handle_table_450cd0 *self, long index, s_bitstream *stream, long reserved_bits);
bool function_989f0(c_handle_table_450cd0 *self, s_bitstream *stream, long index, long a3, long reserved_bits);

#include "unknown_096ed0.h"

class c_replication_view_storage
{
public:
	c_replication_view_storage();
	long unknown00;
	c_vtable_450cf4 aggregate;
	long unknown2c;
	c_handle_table_450cd0 handles;
	c_vtable_450d1c sender;
	c_vtable_450c94 updates;
	c_vtable_450d14 source;
};

#endif
