// @flags /O2 /Gr
/* UNKNOWN_0B3570.CPP: the reference counted start and stop of the session
   search list of unknown_0b35e0.cpp (its entries are allocated through the
   allocator given at the first start) (lane H) */

#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include "unknown_059ad0.h"
#include "online_attributes.h"
#include "unknown_07aec0.h"
#include "network_qos.h"
#include <xtl.h>
#include <string.h>

void function_6b640(long task_index);
void qos_release(long handle);
void function_07ad50(byte *buffer);
long online_match_search(s_range_input const *input);

struct s_system_link_globals
{
	bool active;
	byte unknown01[7];
	s_session_id session_id;
};

#include "system_link_globals.h"
void *g_4d8eb0;

struct s_network_message_session_query
{
	word identifier;
	byte unknown02[2];
	s_session_id session_id;
};

bool __stdcall function_07b140(void *link, long address, long type, long size, void *message);

PRIVATE __forceinline long search_time(void)
{
	return g_510548 ? g_51054c : GetTickCount();
}

struct s_online_search_globals
{
	bool active;
	byte unknown01[3];
	s_range_input input;
	long task_index;
	long qos_handle;
	long capacity;
	void *entries;
	long start_time;
};

s_online_search_globals g_4d8ecc = { 0 };

#define g_4d8ed0 g_4d8ecc.input
#define g_4d8ef0 g_4d8ecc.task_index
#define g_4d8ef4 g_4d8ecc.qos_handle
#define g_4d8ef8 g_4d8ecc.capacity
#define g_4d8efc g_4d8ecc.entries
#define g_4d8f00 g_4d8ecc.start_time

__declspec(noinline) void function_b31b0(void);

// @retail 0xb31b0
void function_b31b0(void)
{
	if (g_4d8ef0 != NONE)
	{
		function_6b640(g_4d8ef0);
		g_4d8ef0 = NONE;
	}
	if (g_4d8ef4 != NONE)
	{
		qos_release(g_4d8ef4);
		g_4d8ef4 = NONE;
	}
	g_4d8ecc.active = false;
	g_4d8ef8 = 0;
	g_4d8efc = NULL;
}

// @retail 0xb2e30
bool function_b2e30(void *entries, long count)
{
	if (!g_4d8eb4.active && g_transport_globals.initialized && g_transport_globals.started)
	{
		g_4d8eb4.active = true;
		g_4d8eb4.unknown01[0] = 0;
		*(dword *)&g_4d8eb4.unknown01[3] = 0;
		function_07ad50((byte *)&g_4d8eb4.session_id);
		g_4d8ec4 = count;
		g_4d8ec8 = entries;
		// Re-read the stored count so its global write stays ahead of the entry clear.
		memset(entries, 0, (*(long const *)&g_4d8ec4) * 0x784);
	}
	return g_4d8eb4.active;
}

// @retail 0xb3100
bool function_b3100(void *entries, long count)
{
	if (!g_4d8ecc.active)
	{
		memset(&g_4d8ed0, 0, sizeof(g_4d8ed0));
		g_4d8ed0.x = NONE;
		g_4d8ef0 = online_match_search(&g_4d8ed0);
		if (g_4d8ef0 != NONE)
		{
			g_4d8ecc.active = true;
			g_4d8ef8 = count;
			g_4d8efc = entries;
			memset(entries, 0, count * 0x784);
		}
		g_4d8f00 = g_510548 ? g_51054c : GetTickCount();
	}
	return g_4d8ecc.active;
}

// @retail 0xb2ea0
void function_b2ea0(void)
{
	if (g_4d8eb4.active)
	{
		long last_query = *(long const *)&g_4d8eb4.unknown01[3];
		if ((long)(search_time() - last_query) > 1500)
		{
			s_network_message_session_query query;
			memset(&query, 0, sizeof(query));
			query.identifier = 2;
			query.session_id = g_4d8eb4.session_id;
			s_type_99af70 address;
			address.address_length = 4;
			address.ipv4_address = 0xffffffff;
			address.port = 1001;
			function_07b140(g_4d8eb0, (long)&address, 2, sizeof(query), &query);
			long now = search_time();
			*(long *)&g_4d8eb4.unknown01[3] = now;
		}
		for (long i = 0; i < g_4d8ec4; i++)
		{
			byte *entry = (byte *)g_4d8ec8 + i * 0x784;
			if (entry[0])
            {
                long previous_time = *(long *)(entry + 4);
                long current_time = search_time();
                if ((long)(current_time - previous_time) > 2000)
                {
                    memset(entry, 0, 0x784);
                    g_4d8eb4.unknown01[0] = 1;
                }
            }
		}
	}
}

// @retail 0xb2fc0
void __stdcall function_0b2fc0(s_network_message_session_query const *message)
{
	if (g_4d8eb4.active)
	{
		byte const *reply = (byte const *)message;
		long free_index = NONE;
		long match = NONE;
		long replacement = NONE;
		for (long i = 0; i < g_4d8ec4; i++)
		{
			byte *entry = (byte *)g_4d8ec8 + i * 0x784;
			if (!entry[0])
			{
				if (free_index == NONE)
					free_index = i;
			}
			else if (memcmp(entry + 0xe0, reply + 0x7c, 0x24) == 0)
			{
				match = i;
				break;
			}
			else if (*(short const *)(reply + 0xaa) != 0 && *(short const *)(reply + 0x20) == 0 &&
				*(short const *)(reply + 0xaa) < *(short *)(entry + 0x10e))
				replacement = i;
		}
		long index = match != NONE ? match : (free_index != NONE ? free_index : (replacement != NONE ? replacement : NONE));
		if (index != NONE)
		{
			byte *entry = (byte *)g_4d8ec8 + index * 0x784;
			if (index != match)
				memset(entry, 0, 0x784);
			if (memcmp(entry + 0x70, reply + 0xc, 0x714) != 0)
			{
				memcpy(entry + 0x70, reply + 0xc, 0x714);
				entry[0x6c] = 1;
				g_4d8eb4.unknown01[0] = 1;
			}
			entry[0] = 1;
			*(long *)(entry + 4) = search_time();
			entry[0x44] = 1;
		}
	}
}

long g_4d8f04;
bool g_4d8f08;
long g_4d8f0c;
c_data_allocator *g_4d8f10;

struct s_0b35e0_entry;
extern long g_4d8f14;
extern s_0b35e0_entry *g_4d8f18;

bool function_b3610(void);
__declspec(noinline) void function_b3670(void);

// @retail 0xb3610
bool function_b3610(void)
{
	bool result = false;
	if (!g_4d8f18)
		g_4d8f18 = (s_0b35e0_entry *)g_4d8f10->allocate(g_4d8f0c * 0x784);
	if (g_4d8f18)
	{
		g_4d8f14 = g_4d8f0c;
		if (g_4d8f08)
			result = function_b3100(g_4d8f18, g_4d8f0c);
		else
			result = function_b2e30(g_4d8f18, g_4d8f0c);
	}
	return result;
}

// @retail 0xb3670
void function_b3670(void)
{
	if (g_4d8f08)
		function_b31b0();
	else if (g_4d8eb4.active)
		g_4d8eb4.active = false;
	if (g_4d8f18)
	{
		g_4d8f10->deallocate(g_4d8f18);
		g_4d8f18 = NULL;
		g_4d8f14 = 0;
	}
}

struct s_long7
{
	long values[7];
};

struct s_online_match_result
{
	XNKEY key;
	XNKID id;
	XNADDR address;
	DWORD public_filled;
	DWORD public_open;
	DWORD private_filled;
	DWORD private_open;
	s_long7 properties;
};

struct s_qos_target
{
	XNKID kid;
	XNKEY key;
	XNADDR xna;
};

struct s_qos_handle
{
	short salt;
	short state;
	XNQOS *qos;
};

struct s_online_match_session_info;
void session_search_result_initialize(s_online_match_session_info const *record, s_0b35e0_entry *entry);
void function_8fb30(long task_index, s_online_match_result *output, word *capacity);
long online_task_poll(long task_index);
long qos_lookup(long kind, long count, long bits_per_second, s_qos_target *targets);
long qos_target_status(long handle, long index);
bool session_description_decode_buffer(byte const *data, long size, void *description);

PRIVATE __forceinline s_qos_handle *search_qos_handle(long handle)
{
	s_qos_handle *result = NULL;
	if (g_4cf8d4 && handle != NONE)
	{
		s_record_pool *data = g_4cf8d8;
		long index = handle & 0xffff;
		if (index < data->high_water_index)
		{
			s_qos_handle *element = (s_qos_handle *)(data->data + data->size * index);
			if (element->salt && element->salt == (handle >> 16))
				result = element;
		}
	}
	return result;
}

// @retail 0xb3200
// score 73 (was 73): the count, the loop index and the entry pointer take the
// three frame slots in the other order, and retail reads the qos result fields
// through the entry pointer instead of keeping the result in ebp
void function_b3200(void)
{
	union
	{
		s_online_match_result results[64];
		s_qos_target targets[64];
	} buffer;
	byte description[0x714];
	if (g_4d8ef0 != NONE)
	{
		switch (online_task_poll(g_4d8ef0))
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
		{
			long capacity = g_4d8ef8;
			function_8fb30(g_4d8ef0, buffer.results, (word *)&capacity);
			memset(g_4d8efc, 0, g_4d8ef8 * 0x784);
			long count = (word)capacity;
			for (long i = 0; i < count; i++)
				session_search_result_initialize((s_online_match_session_info const *)&buffer.results[i],
					(s_0b35e0_entry *)((byte *)g_4d8efc + i * 0x784));
			if (g_4d8ef4 != NONE)
			{
				qos_release(g_4d8ef4);
				g_4d8ef4 = NONE;
			}
			if ((word)capacity > 0)
			{
				for (long i = 0; i < count; i++)
				{
					byte *entry = (byte *)g_4d8efc + i * 0x784;
					buffer.targets[i].kid = *(XNKID *)(entry + 8);
					buffer.targets[i].key = *(XNKEY *)(entry + 0x10);
					buffer.targets[i].xna = *(XNADDR *)(entry + 0x20);
				}
				g_4d8ef4 = qos_lookup(0, count, 65536, buffer.targets);
			}
		}
		default:
			if (g_4d8ef0 != NONE)
			{
				function_6b640(g_4d8ef0);
				g_4d8ef0 = NONE;
			}
			break;
		}
	}
	if (g_4d8ef4 != NONE)
	{
		long count = 0;
		s_qos_handle *lookup = search_qos_handle(g_4d8ef4);
		if (lookup)
			count = lookup->qos->cxnqos;
		for (long i = 0; i < count; i++)
		{
			long status = qos_target_status(g_4d8ef4, i);
			byte *entry = (byte *)g_4d8efc + i * 0x784;
			if (status != *(long *)(entry + 0x48))
			{
				*(long *)(entry + 0x48) = status;
				if (status == 2 || status == 5)
				{
					entry[0x44] = true;
					s_qos_result *result = (s_qos_result *)(entry + 0x4c);
					entry[0x45] = qos_target_result(g_4d8ef4, result, i);
					if (entry[0x45] && result->data_size > 0 && session_description_decode_buffer(result->data, result->data_size, description))
					{
						memcpy(entry + 0x70, description, sizeof(description));
						entry[0x6c] = true;
					}
					result->data_size = 0;
					result->data = NULL;
				}
				else if (status == 4)
					entry[0x44] = true;
			}
		}
		lookup = search_qos_handle(g_4d8ef4);
		if (!lookup || lookup->qos->cxnqosPending == 0)
		{
			if (g_4d8ef4 != NONE)
			{
				qos_release(g_4d8ef4);
				g_4d8ef4 = NONE;
			}
		}
	}
}

// @retail 0xb3570
bool function_b3570(long count, c_data_allocator *allocator, bool flag)
{
	bool result;

	if (g_4d8f04 <= 0)
	{
		g_4d8f0c = count;
		g_4d8f08 = flag;
		g_4d8f10 = allocator;
	}
	result = function_b3610();
	if (result)
		g_4d8f04++;
	return result;
}

// @retail 0xb35a0
void function_b35a0(void)
{
	g_4d8f04--;
	if (g_4d8f04 <= 0)
	{
		function_b3670();
		g_4d8f0c = 0;
		g_4d8f10 = NULL;
	}
}

// @retail 0xb35d0
void function_b35d0(bool start)
{
	if (start)
		function_b3610();
	else
		function_b3670();
}
