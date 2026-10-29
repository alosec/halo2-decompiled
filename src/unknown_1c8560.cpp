#include "unknown_11c920.h"
#include "globals.h"
#include "slot_handler.h"
#include "data_array.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_1c8560
{
	short field_0;
	byte field_2[2];
	long field_4;
	byte field_8[0xc];
	short field_14;
};

extern s_record_pool *g_5044c8;
void function_1e2a90(long actor_index);
void __stdcall function_b83b0(long object_index, bool unused);
void __stdcall function_b8460(long object_index, bool detach);
void __stdcall swarm_delete(long arg_0);

struct s_1c8560_iterator
{
	byte *datum;
	s_record_pool_iterator records;
	long object_index;
};

/* steps the iterator while g_4f55d0 is active (an inline copy of the
   record pool step, with the found datum kept apart until the end) */
PRIVATE __forceinline byte *function_1c8561(s_1c8560_iterator *arg_0)
{
    byte *datum = NULL;
    s_record_pool *data;
    long index;

    if (!g_4f55d0->active)
        return NULL;
    data = arg_0->records.data;
    index = data_next_absolute_index_inlined(data, arg_0->records.index + 1);
    if (index != NONE)
    {
        byte *result = data->data + data->size * index;
        arg_0->records.index = index;
        arg_0->records.datum_index = (*(short *)result << 16) | index;
        datum = result;
    }
    else
    {
        arg_0->records.index = data->maximum_count;
        arg_0->records.datum_index = NONE;
    }
    return datum;
}

// score 67 (was 84): the iterator step returns the datum and the loop tests the pointer, as in
// retail, but the block that computes the found datum is laid out after the exit test here and
// before it in retail, which moves the two exits of the loop.
// Earlier: actor arm first 64 at 364 bytes; with the step's arms swapped too 88 at 369
// The control flow is the same in both builds (the scan's failure branches and its exhaustion
// reach the not-found block, the scan's jne reaches the found block, and the found block re-tests
// the index and jumps back to the not-found block); only the place of the found block differs:
// retail puts it between the not-found block and the shared test (0x1c85d7 falling into
// 0x1c85f4), ours puts it after the loop body.
// Tried: actor arm first 64 at 364 bytes; with the step's arms swapped too 88 at 369
// score 67 (was 67): actor arm first 64 at 364 bytes; with the step's arms swapped too 88 at 369
// (found block where retail has it, but the not-found block is copied after the test where
// retail jumps back to it); store orders, early return, inline, data_find_index,
// data_iterator_next_inlined, a bool step: none better.
// Round 406: shared datum-index producer, ordinary/direct pool observations,
// original/common inactive exit, simple/actor arm first 68/63/132/132/68/63/132/132.
// Best 63 at 363/361 is below the eight-point gate; all restored.
// @retail 0x1c8560
long __stdcall function_1c8560(long arg_0, void *arg_1, long arg_2, bool *arg_3, void *arg_4, long arg_5)
{
	s_1c8560 *local_2;
	s_1c8560_iterator local_1;
	long local_0 = 0;
	if (g_4f55d0->active)
	{
		local_1.records.data = g_5044c8;
		local_1.records.index = NONE;
	}
	while ((local_1.datum = function_1c8561(&local_1)) != NULL)
	{
		local_2 = (s_1c8560 *)local_1.datum;
		long local_7 = local_2->field_4;
		if (local_7 != NONE)
		{
			s_actor_view *actor = actor_get(local_7);
			if (actor->unknown009 || *(long *)((byte *)actor + 0x10) == NONE)
				continue;
		}
		local_0 += local_2->field_14;
		if (local_7 == NONE)
			swarm_delete(local_1.records.datum_index);
		else
		{
			s_actor_view *local_4 = actor_get(local_7);
			long local_5 = local_4->unknown018;
			long local_6 = *(long *)((byte *)local_4 + 0x1c);
			function_1e2a90(local_7);
			if (local_5 != NONE)
			{
				function_b83b0(local_5, true);
				function_b8460(local_5, true);
			}
			else if (local_6 != NONE)
				swarm_delete(local_6);
		}
	}
	*arg_3 = false;
	return local_0;
}

