#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include "crc.h"
#include "unknown_123b30.h"

// @flags /O2 /Ob1 /Gr

struct s_audio_queue_node
{
	byte unknown00[4];
	long object_index;
	byte unknown08[0x44 - 8];
	long parent;
	bool release;
	byte unknown49[7];
	long next;
};

struct s_audio_queue
{
	byte unknown000[0x7d0];
	long first;
	short count;
	byte unknown7d6[2];
	long unknown7d8;
};

struct s_audio_linked_object
{
	byte unknown000[0x138];
	short team;
	byte unknown13a[0x342 - 0x13a];
	short link_offset;
};

struct s_audio_linked_header
{
	byte unknown00[8];
	s_audio_linked_object *object;
};

struct s_audio_object_link
{
	byte unknown00[0x1c];
	long node_index;
};

struct s_node_owner;
void function_20fe20(s_node_owner *owner);
long __declspec(noinline) function_20f040(short team);
extern s_record_pool *g_4f9398;
s_audio_queue *g_4f939c;

// @retail 0x20b930
void function_20b930(void)
{
	long size = 2 * sizeof(s_audio_queue);
	byte *memory = (byte *)game_state_globals.base_address + game_state_globals.cpu_allocation_size;
	game_state_globals.cpu_allocation_size += size;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));
	g_4f939c = (s_audio_queue *)memory;
	g_4f9398 = data_new_inlined("vocalization records", 15, sizeof(s_audio_queue_node), 0, g_510c2c);
}

PRIVATE __forceinline s_audio_queue_node *audio_queue_node(long index)
{
	return (s_audio_queue_node *)(g_4f9398->data + (index & 0xffff) * sizeof(s_audio_queue_node));
}

// @retail 0x20b9b0
void function_20b9b0(void)
{
	memset(g_4f939c, 0, 2 * sizeof(s_audio_queue));
	g_4f9398->valid = true;
	record_pool_release_all(g_4f9398);
	long i = 0;
	do
	{
		g_4f939c[i].first = NONE;
		g_4f939c[i].count = 0;
		g_4f939c[i].unknown7d8 = NONE;
		i++;
	}
	while (i < 2);
}

// @retail 0x20f8b0
bool function_20f8b0(s_audio_queue *queue, long node_index)
{
	bool result = false;
	long *link = &queue->first;
	while (*link != NONE)
	{
		long current = *link;
		s_audio_queue_node *node = audio_queue_node(current);
		if (current == node_index)
		{
			*link = node->next;
			queue->count--;
			result = true;
			break;
		}
		link = &node->next;
	}
	return result;
}

// @retail 0x20f910
void function_20f910(long node_index)
{
	s_record_pool *pool = g_4f9398;
	s_audio_queue_node *node = audio_queue_node(node_index);
	long object_index = ((s_audio_queue_node volatile *)node)->object_index;
	if (object_index != NONE)
	{
		s_audio_linked_object *object = ((s_audio_linked_header *)g_4e0300->data)[object_index & 0xffff].object;
		s_audio_object_link *link = (s_audio_object_link *)((byte *)object + object->link_offset);
		if (link->node_index == node_index)
			link->node_index = NONE;
	}
	record_pool_release(pool, node_index);
}

// @retail 0x210130
void function_210130(short queue_index)
{
	s_audio_queue *queue = (s_audio_queue *)((byte *)g_4f939c + queue_index * sizeof(s_audio_queue));
	for (long index = queue->first; index != NONE; )
	{
		s_audio_queue_node *node = audio_queue_node(index);
		node->release = true;
		index = node->next;
	}
	function_20fe20((s_node_owner *)queue);
}

// @retail 0x20fd50
void function_20fd50(long object_index, long node_index)
{
	if (node_index != NONE)
	{
		s_audio_linked_object *object = ((s_audio_linked_header *)g_4e0300->data)[object_index & 0xffff].object;
		long team = object->team;
		long queue_index = (short)function_20f040(team);
		if (function_20f8b0(&g_4f939c[queue_index], node_index))
			function_20f910(node_index);
	}
}

// @retail 0x210180
void __stdcall function_210180(long value)
{
	bool update = g_4e6948->flag1121;
	for (short i = 0; i < 2; i++)
	{
		function_210130(i);
		if (update)
			g_4f939c[i].unknown7d8 = value;
	}
}

// @retail 0x20fda0
void function_20fda0(long node_index)
{
	s_audio_queue_node *node = audio_queue_node(node_index);
	long index = node->next;
	node->release = true;
	while (index != NONE)
	{
		node = audio_queue_node(index);
		s_audio_queue_node *parent = (s_audio_queue_node *)datum_get_inlined(g_4f9398, node->parent);
		if (parent && parent->release)
			node->release = true;
		index = node->next;
	}
}

PRIVATE __forceinline void release_audio_queue_node(long node_index)
{
	s_record_pool *pool = g_4f9398;
	s_audio_queue_node *node = audio_queue_node(node_index);
	long object_index = ((s_audio_queue_node volatile *)node)->object_index;
	if (object_index != NONE)
	{
		s_audio_linked_object *object = ((s_audio_linked_header *)g_4e0300->data)[object_index & 0xffff].object;
		s_audio_object_link *link = (s_audio_object_link *)((byte *)object + object->link_offset);
		if (link->node_index == node_index)
			link->node_index = NONE;
	}
	record_pool_release(pool, node_index);
}

// @retail 0x20ff50
void function_20ff50(long object_index)
{
	long const *object_reference = &object_index;
	s_audio_linked_object *object = ((s_audio_linked_header *)g_4e0300->data)[*object_reference & 0xffff].object;
	long team = object->team;
	short queue_index = function_20f040(team);
	s_audio_queue *queue = &g_4f939c[queue_index];
	for (long index = queue->first; index != NONE; )
	{
		s_audio_queue_node *node = audio_queue_node(index);
		long next = node->next;
		(void)&next;
		if (node->object_index == *object_reference && function_20f8b0(queue, index))
			release_audio_queue_node(index);
		index = next;
	}
}

// @retail 0x210020
void function_210020(long object_index)
{
	long const *object_reference = &object_index;
	byte *header = g_4e0300->data + (*object_reference & 0xffff) * 12;
	if ((1 << header[3]) & 0x1023)
	{
		long i = 0;
		do
		{
			s_audio_queue *queue = &g_4f939c[i];
			for (long index = queue->first; index != NONE; )
			{
				s_audio_queue_node *node = audio_queue_node(index);
				long next = node->next;
				if ((node->object_index == *object_reference || *(long *)((byte *)node + 0x28) == *object_reference) &&
					function_20f8b0(queue, index))
					release_audio_queue_node(index);
				index = next;
			}
			i++;
		} while (i < 2);
	}
}
