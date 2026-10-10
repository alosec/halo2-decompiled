#include "async.h"

#include <stddef.h>

s_async_globals async_globals;
s_thread_stack g_5020c8;
s_job_node *g_510810;
dword g_46e488 = 8;
static s_job_node *g_removed_free_node;
static byte g_async_read_buffer[16];
static dword g_async_read_bytes;
static bool volatile g_async_read_done;
static byte g_async_read_expected;

long function_213760(dword location, long size, void *buffer, dword *bytes_read,
	bool *done, long type, long priority);

extern "C" void *h2_guest_memset(void *destination, int value, size_t count)
{
	unsigned char *bytes = (unsigned char *)destination;
	for (size_t i = 0; i < count; i++)
		bytes[i] = (unsigned char)value;
	return destination;
}

extern "C" void *h2_guest_memcpy(void *destination, void const *source,
	size_t count)
{
	byte *output = (byte *)destination;
	byte const *input = (byte const *)source;
	for (size_t i = 0; i < count; i++)
		output[i] = input[i];
	return destination;
}

static int submit_read(dword location, byte expected)
{
	h2_guest_memset(g_async_read_buffer, 0, sizeof(g_async_read_buffer));
	g_async_read_bytes = 0;
	g_async_read_done = false;
	g_async_read_expected = expected;
	return function_213760(location, sizeof(g_async_read_buffer),
		g_async_read_buffer, &g_async_read_bytes, (bool *)&g_async_read_done,
		7, 3) != NONE;
}

extern "C" int h2_async_submit_active_read(void)
{
	return submit_read(0x00000800u, 0xa0u);
}

extern "C" int h2_async_submit_shared_read(void)
{
	return submit_read(0x80000800u, 0xc0u);
}

extern "C" int h2_async_submit_campaign_read(void)
{
	return submit_read(0x00000800u, 0xe0u);
}

extern "C" int h2_async_read_completed(void)
{
	if (!g_async_read_done || g_async_read_bytes != sizeof(g_async_read_buffer))
		return 0;
	for (dword i = 0; i < sizeof(g_async_read_buffer); i++)
	{
		if (g_async_read_buffer[i] != (byte)(g_async_read_expected + i))
			return 0;
	}
	return 1;
}

extern "C" int h2_async_queue_free_count(void)
{
	bool seen[150];
	for (long i = 0; i < 150; i++)
		seen[i] = false;
	long count = 0;
	for (s_job_node *node = async_globals.free_list; node; node = node->next)
	{
		if (node < &async_globals.nodes[0] || node >= &async_globals.nodes[150])
			return -1;
		long index = (long)(node - &async_globals.nodes[0]);
		if (seen[index] || ++count > 150)
			return -1;
		seen[index] = true;
	}
	return count;
}

extern "C" int h2_async_queue_remove_free_head(void)
{
	if (g_removed_free_node || !async_globals.free_list)
		return 0;
	g_removed_free_node = async_globals.free_list;
	async_globals.free_list = g_removed_free_node->next;
	g_removed_free_node->next = NULL;
	return 1;
}

extern "C" int h2_async_queue_restore_free_head(void)
{
	if (!g_removed_free_node)
		return 0;
	g_removed_free_node->next = async_globals.free_list;
	async_globals.free_list = g_removed_free_node;
	g_removed_free_node = NULL;
	return 1;
}

extern "C" int h2_async_queue_is_cleared(void)
{
	return !async_globals.thread && !async_globals.free_list_mutex &&
		!async_globals.work_list_mutex && !async_globals.work_semaphore &&
		!async_globals.free_list && !async_globals.work_list && !g_510810;
}
