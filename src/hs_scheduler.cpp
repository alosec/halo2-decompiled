// @flags /O2 /Ob1 /Gr
/* HS_SCHEDULER.CPP: the script runtime lifetime and scheduling (the
   /O2 /Ob1 /Gr half; hs_evaluation.cpp has the rest): creating its data
   arrays, loading and releasing the syntax nodes, threads and global values
   of a map, running the threads that are due each tick, and finding the
   thread of a script */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "hs_scheduler.h"

/* releases the threads and the global values of the scenario (the external
   globals keep theirs) and stops the threads running */
// @retail 0x209290
void hs_scheduler_map_end(void)
{
	g_4f9384->valid = false;
	s_record_pool_iterator iterator;
	iterator.data = g_4f9380;
	iterator.index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		long global_value_index = iterator.datum_index;
		if ((global_value_index & 0xffff) >= k_hs_engine_global_count)
			record_pool_release(iterator.data, global_value_index);
	}
	g_4f9388 = false;
}

void function_209850(long thread_index); /* runs a thread; unknown_209520.cpp */

/* runs a thread for its command script if it is due: 0 when it finished,
   1 when it went back to waiting for its command script, 2 when it sleeps
   or was not due */
// @retail 0x209580
short hs_thread_run_if_due(long thread_index)
{
	short result = 2;
	s_record_pool *threads = g_4f9384;
	s_hs_thread *thread = (s_hs_thread *)(threads->data + (thread_index & 0xffff) * sizeof(s_hs_thread));

	if (thread->sleep_until == k_hs_thread_sleep_command_script)
	{
		thread->sleep_until = 0;
	}
	if (thread->sleep_until >= 0 && thread->sleep_until <= g_510c54->game_time)
	{
		function_209850(thread_index);
		result = 0;
		if (thread->sleep_until != k_hs_thread_finished)
		{
			result = (thread->sleep_until != k_hs_thread_sleep_command_script) + 1;
		}
	}
	return result;
}

/* the datum index of the first used element after index (NONE: from the
   start) */
PRIVATE inline long hs_data_iterate_next(s_record_pool *data, long index)
{
	long next = index == NONE ? 0 : (index & 0xffff) + 1;
	return data_datum_index(data, function_16bc00(data, next));
}

void object_lists_garbage_collect(void);

/* runs every thread that is due, except the static script calls, while the
   threads run; then collects the unused object lists */
// @retail 0x209310
void hs_scheduler_update(void)
{
	if (g_4f9388)
	{
		long current_time = g_510c54->game_time;
		s_record_pool *threads = g_4f9384;
		long thread_index = hs_data_iterate_next(threads, NONE);
		do
		{
			if (thread_index == NONE)
				break;
			long absolute_index = thread_index & 0xffff;
			s_hs_thread *thread = (s_hs_thread *)(threads->data + absolute_index * sizeof(s_hs_thread));
			if (thread->type != _hs_thread_kind_static_script && thread->sleep_until >= 0 && thread->sleep_until <= current_time)
			{
				function_209850(thread_index);
				threads = g_4f9384;
			}
			thread_index = data_datum_index(threads, function_16bc00(threads, absolute_index + 1));
		} while (g_4f9388);
		object_lists_garbage_collect();
	}
}

/* the thread of a script, or NONE */
// @retail 0x209e70
long hs_thread_for_script(short script_index)
{
	s_record_pool *threads = g_4f9384;
	for (long thread_index = hs_data_iterate_next(threads, NONE); thread_index != NONE; thread_index = hs_data_iterate_next(threads, thread_index))
	{
		s_hs_thread *thread = (s_hs_thread *)(threads->data + (thread_index & 0xffff) * sizeof(s_hs_thread));
		if (thread->script_index == script_index)
			return thread_index;
	}
	return NONE;
}

int function_11c920(char const *left, char const *right);

/* the thread of the script with this name, or NONE */
// @retail 0x209f00
long __stdcall hs_thread_for_script_name(char const *name)
{
	s_record_pool *threads = g_4f9384;
	long thread_index = data_datum_index(threads, function_16bc00(threads, 0));
	while (thread_index != NONE)
	{
		s_hs_thread *thread = &((s_hs_thread *)threads->data)[thread_index & 0xffff];
		if (thread->script_index != NONE)
		{
			char const *script_name = ((s_hs_scenario *)g_4e0350)->scripts[thread->script_index].name;
			if (!function_11c920(script_name, name))
				return thread_index;
			threads = g_4f9384;
		}
		long next = thread_index == NONE ? 0 : (thread_index & 0xffff) + 1;
		thread_index = data_datum_index(threads, data_next_absolute_index_inlined(threads, next));
	}
	return NONE;
}

/* the object lists of the script engine: their headers ("object list
   header") and the references they chain ("list object") */
extern s_record_pool *g_4f55d8;
extern s_record_pool *g_4f55d4;
void hs_temporary_data_release(void);

/* detaches a data array from its storage, once */
PRIVATE __forceinline void hs_data_disconnect(s_record_pool *data)
{
	if (!data->flag1)
	{
		data->flag1 = true;
		data->data = NULL;
		data->valid = false;
	}
}

/* end-of-map step of the subsystem: releases the threads, syntax
   nodes, global values and object lists of the map */
// @retail 0x20a7d0
void hs_map_end(void)
{
	hs_temporary_data_release();
	hs_data_disconnect(g_4f9394);
	hs_scheduler_map_end();
	g_4f55d8->valid = false;
	g_4f55d4->valid = false;
}

/* releases the expression threads run from outside, and every syntax node
   not marked permanent */
// @retail 0x20a850
void hs_temporary_data_release(void)
{
	if (g_4f9388)
	{
		s_record_pool *threads = g_4f9384;
		for (long thread_index = hs_data_iterate_next(threads, NONE); thread_index != NONE; thread_index = hs_data_iterate_next(threads, thread_index))
		{
			s_hs_thread *thread = (s_hs_thread *)(threads->data + (thread_index & 0xffff) * sizeof(s_hs_thread));
			if (thread->type == _hs_thread_kind_runtime_evaluate)
				record_pool_release(threads, thread_index);
		}
	}
	s_record_pool *expression_nodes = g_4f9394;
	for (long node_index = hs_data_iterate_next(expression_nodes, NONE); node_index != NONE; )
	{
		s_hs_expression_node *node = (s_hs_expression_node *)(expression_nodes->data + (node_index & 0xffff) * sizeof(s_hs_expression_node));
		if (!(node->flags & _hs_expression_node_flag_permanent))
			record_pool_release(expression_nodes, node_index);
		long next = node_index == NONE ? 0 : (node_index & 0xffff) + 1;
		node_index = data_datum_index(expression_nodes, data_next_absolute_index_inlined(expression_nodes, next));
	}
}


extern long g_4686c0; /* the index of the loaded scenario, NONE when none is */
void function_2a09c0(void);
void function_2090a0(void);

/* start-of-map step of the subsystem: loads the syntax nodes of the
   scenario, points their string constants into its string data, empties
   the object lists and starts the threads */
// @retail 0x20a750
void hs_map_begin(void)
{
    s_hs_scenario *scenario = (s_hs_scenario *)g_4e0350;
    if (g_4686c0 == NONE ? NULL : scenario)
    {
        if (g_4f9394->flag1)
            function_16b6b0(g_4f9394, scenario->expression_node_count, (byte *)scenario->expression_nodes);
        if (scenario->script_count || scenario->block_1d0_count <= 0)
            function_2a09c0();
    }
    g_4f55d8->valid = true;
    record_pool_release_all(g_4f55d8);
    g_4f55d4->valid = true;
    record_pool_release_all(g_4f55d4);
    function_2090a0();
}


void function_20a1a0(void);
long function_16b990(s_record_pool *data, long index);

/* creates the threads and the global values, and reserves the values of
   the external globals */
// @retail 0x208ff0
void hs_scheduler_init(void)
{
    g_4f9384 = data_new_inlined("hs thread", k_maximum_hs_threads, sizeof(s_hs_thread), 0, g_510c2c);
    g_4f9380 = data_new_inlined("hs globals", k_maximum_hs_globals, 8, 0, g_510c2c);
    if (g_4f9384 && g_4f9380)
    {
        g_4f9380->valid = true;
        record_pool_release_all(g_4f9380);
        for (long global_index = 0; global_index < k_hs_engine_global_count; global_index++)
            function_16b990(g_4f9380, global_index);
    }
    function_20a1a0();
}


void function_1ded00(void);


/* a data array with its header and occupancy bitmap in one allocation,
   its elements supplied later */
PRIVATE inline s_record_pool *hs_data_new_unattached(char const *name, long count, long size, c_data_allocator *allocator)
{
    long bitmap_size = ((count + 31) >> 5) * sizeof(dword);
    s_record_pool *data = (s_record_pool *)allocator->allocate(sizeof(s_record_pool) + bitmap_size);
    if (data)
        data_array_construct(data, name, count, size, 0, allocator, (dword *)(data + 1));
    return data;
}

/* initialize of the subsystem: creates the syntax nodes, the object lists
   and the runtime, then sets up for the loaded map */
// @retail 0x20a700
void hs_init(void)
{
    g_4f9394 = hs_data_new_unattached("script node", k_maximum_hs_expression_nodes, sizeof(s_hs_expression_node), g_468758);
    function_1ded00();
    hs_scheduler_init();
    hs_map_begin();
}
