// @flags /O2 /Gr
/* DATA_ITERATOR.CPP: the data iterator that remembers its current datum, used
   all over the game (lane D) */

#include "unknown_11c920.h"
#include "data_array.h"

// @retail 0x6b380
bool data_datum_iterator_next(s_data_datum_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = function_16bc00(data, iterator->index + 1);
	byte *datum;

	if (index != NONE)
	{
		datum = data->data + data->size * index;
		iterator->index = index;
		iterator->datum_index = (*(short *)datum << 16) | index;
	}
	else
	{
		iterator->index = data->maximum_count;
		iterator->datum_index = NONE;
		datum = 0;
	}
	iterator->datum = datum;
	return datum != 0;
}

#if H2_NATIVE_PORT
/* native: emit the inline-declared iterator so the guest image can link it */
__attribute__((used)) static void *h2_emit_data_datum_iterator_next = (void *)&data_datum_iterator_next;
#endif
