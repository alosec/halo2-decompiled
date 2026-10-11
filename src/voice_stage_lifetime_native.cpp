// Exact constructor-free physical source, registered voice stage, secondary
// menu pool, and ordered disposal lifetime.  Allocation and loop ownership
// remain genuine recovered/platform imports; failed allocation is not turned
// into a fabricated pool or engine.

#include "unknown_11c920.h"
#include <stddef.h>

#ifdef H2_NATIVE_PORT

class c_voice_xhv;

struct s_loop_block;

class c_memory_source
{
public:
	typedef void *(*allocate_method)(c_memory_source *, long);
	typedef void (*release_method)(c_memory_source *, void *);
	struct s_table
	{
		allocate_method allocate;
		release_method release;
	};

	s_table const *vtable;
};

class c_physical_memory_source
{
public:
	c_memory_source::s_table const *vtable;
	long unknown04;
};

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

struct s_voice_globals
{
	long unknown00;
	long unknown04;
	long unknown08;
	long unknown0c;
	long mode;
	long session_kind;
	long type;
	long pool_mode;
	long unknown20;
	long unknown24[0x30];
	s_loop_allocator *pool;
	s_loop_allocator *pool2;
	bool use_pool2;
	byte unknowned;
	word unknownee;
	word unknownf0[16];
	word unknown110[16];
	long port_states[4];
	bool initialized;
	byte unknown141[3];
};

struct c_voice_xhv_view
{
	void *vtable;
	bool initialized;
	byte unknown05[3];
	long mode;
	long unknown0c;
	void *engine;
};

struct s_physical_memory_globals
{
	long stage;
	long base_address;
	long maximum_address;
	long lower[5];
	long upper[5];
};

static_assert(sizeof(s_physical_memory_globals) == 0x34,
	"physical memory globals size");
static_assert(sizeof(c_memory_source) == 0x04, "memory source size");
static_assert(sizeof(c_physical_memory_source) == 0x08,
	"physical memory source size");
static_assert(sizeof(s_loop_allocator) == 0x40, "loop allocator size");
static_assert(offsetof(s_loop_allocator, source) == 0x24,
	"loop source offset");
static_assert(offsetof(s_loop_allocator, field3c) == 0x3c,
	"loop flags offset");
static_assert(sizeof(s_voice_globals) == 0x144, "voice globals size");
static_assert(offsetof(s_voice_globals, type) == 0x18,
	"voice type offset");
static_assert(offsetof(s_voice_globals, pool_mode) == 0x1c,
	"voice pool mode offset");
static_assert(offsetof(s_voice_globals, pool) == 0xe4,
	"voice primary pool offset");
static_assert(offsetof(s_voice_globals, pool2) == 0xe8,
	"voice secondary pool offset");
static_assert(offsetof(s_voice_globals, use_pool2) == 0xec,
	"voice pool selector offset");
static_assert(offsetof(c_voice_xhv_view, initialized) == 0x04,
	"voice XHV initialized offset");
static_assert(offsetof(c_voice_xhv_view, mode) == 0x08,
	"voice XHV mode offset");
static_assert(offsetof(c_voice_xhv_view, engine) == 0x10,
	"voice XHV engine offset");

extern s_physical_memory_globals g_global_f9ae07;
extern "C" long __stdcall XPhysicalProtect(
	void *address, dword size, dword protection);
void function_18e250(s_loop_allocator *loop, long size,
	char const *name, c_memory_source *source);
s_loop_allocator *function_18e1f0(c_memory_source *source, long size,
	char const *name);
void function_18e230(s_loop_allocator *loop);
void voice_xhv_reset_masks(c_voice_xhv *xhv);
void voice_xhv_reset_port_modes(c_voice_xhv *xhv);
bool voice_xhv_create(c_voice_xhv *xhv);
void voice_xhv_dispose(c_voice_xhv *xhv);
void voice_start_engine(void);
void voice_stop_engine(void);

extern s_voice_globals g_4c9878;
extern c_voice_xhv g_476fc8;
extern c_memory_source *g_46dd54;

void *h2_physical_memory_source_allocate(c_memory_source *, long size)
	__asm__("?allocate@c_physical_memory_source@@UAEPAXJ@Z");
void *h2_physical_memory_source_allocate(c_memory_source *, long size)
{
	long const stage = g_global_f9ae07.stage;
	long const limit = g_global_f9ae07.lower[stage];
	long *const top = &g_global_f9ae07.upper[stage];
	long const aligned_size = (size + 0xfff) & 0xfffff000;
	long const address = *top - aligned_size;
	if (address < limit)
		return NULL;

	*top = address;
	if (!address)
		return NULL;
	void *const result = reinterpret_cast<void *>(address | 0x80000000u);
	XPhysicalProtect(result, aligned_size, 0x04);
	return result;
}

void h2_physical_memory_source_release(c_memory_source *, void *)
	__asm__("?release@c_physical_memory_source@@UAEXPAX@Z");
void h2_physical_memory_source_release(c_memory_source *, void *)
{
}

extern c_memory_source::s_table const h2_physical_memory_source_vtable =
{
	h2_physical_memory_source_allocate,
	h2_physical_memory_source_release,
};

c_physical_memory_source g_476fbc =
{
	&h2_physical_memory_source_vtable,
	2,
};

extern char const g_voice_pool_name[] = "voice pool";
extern char const g_voice_menu_pool_name[] = "voice y menu pool";

// @retail 0x53310
void function_53310(long stage) __asm__("?function_53310@@YGXJ@Z");
void function_53310(long stage)
{
	long type;
	long size = 0;
	long mode = 0;
	c_memory_source *const source =
		reinterpret_cast<c_memory_source *>(&g_476fbc);
	s_loop_allocator *loop = NULL;

	if (stage > 1)
		type = stage <= 3;
	else
		type = 2;

	switch (type)
	{
	case 1:
		mode = 1;
		break;
	case 2:
		size = 0x78000;
		mode = 2;
		break;
	}

	g_4c9878.type = type;
	g_4c9878.pool_mode = mode;
	if (type == 2)
	{
		loop = static_cast<s_loop_allocator *>(
			source->vtable->allocate(source, size + 0x50));
		if (loop)
			function_18e250(loop, size, g_voice_pool_name, source);
		g_4c9878.pool = loop;
		// Retail treats this physical allocation as mandatory and immediately
		// publishes all three loop flags after construction.
		g_4c9878.pool->field3c = 1;
		g_4c9878.pool->field3d = 1;
		g_4c9878.pool->field3e = 1;

		c_voice_xhv_view *const xhv =
			reinterpret_cast<c_voice_xhv_view *>(&g_476fc8);
		if (!xhv->initialized)
		{
			xhv->engine = NULL;
			xhv->mode = mode;
			voice_xhv_reset_masks(&g_476fc8);
			voice_xhv_reset_port_modes(&g_476fc8);
			xhv->initialized = voice_xhv_create(&g_476fc8);
		}
	}
}

// @retail 0x533e0
void function_533e0(void)
{
	if (g_4c9878.pool)
	{
		voice_xhv_dispose(&g_476fc8);
		function_18e230(g_4c9878.pool);
		g_4c9878.pool = NULL;
	}
}

// @retail 0x53420
void voice_initialize_menu_pool(void)
{
	if (g_4c9878.type == 1)
	{
		g_4c9878.use_pool2 = true;
		g_4c9878.pool2 = function_18e1f0(
			g_46dd54, 0x61800, g_voice_menu_pool_name);
		if (g_4c9878.pool2)
		{
			g_4c9878.pool2->field3c = true;
			g_4c9878.pool2->field3d = true;
			g_4c9878.pool2->field3e = true;
			voice_start_engine();
		}
	}
}

// @retail 0x534a0
void voice_dispose_menu_pool(void)
{
	if (g_4c9878.type == 1)
	{
		if (g_4c9878.pool2)
		{
			voice_stop_engine();
			function_18e230(g_4c9878.pool2);
			g_4c9878.pool2 = NULL;
		}
		g_4c9878.use_pool2 = false;
	}
}

#endif
