#include "async.h"

#include <stddef.h>

s_async_globals async_globals;
s_thread_stack g_5020c8;
s_job_node *g_510810;
static s_job_node *g_removed_free_node;

extern "C" void *h2_guest_memset(void *destination, int value, size_t count)
{
	unsigned char *bytes = (unsigned char *)destination;
	for (size_t i = 0; i < count; i++)
		bytes[i] = (unsigned char)value;
	return destination;
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
