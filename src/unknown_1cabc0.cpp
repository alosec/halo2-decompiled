// @flags /O2 /Gr
/* UNKNOWN_1CABC0.CPP: cluster partitions (0x1cabc0..0x1caed0): for each
   cluster a list of the things in it, and for each thing the list of the
   clusters it touches ("cluster %s" and "%s cluster" data arrays) */

#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include <string.h>

/* a thing's link to one cluster */
struct s_cluster_reference
{
	short identifier;
	byte unknown02[2];
	short cluster_index;
	byte unknown06[2];
	long next_reference_index;
};

/* a cluster's link to one thing */
struct s_data_reference
{
	short identifier;
	byte unknown02[2];
	long data_index;
	long next_reference_index;
};

struct s_cluster_partition
{
	long *cluster_first_data_references;
	s_record_pool *data_references;
	s_record_pool *cluster_references;
};

inline byte *cluster_partition_datum(s_record_pool *data, long index)
{
	return data->data + (index & 0xffff) * data->size;
}

// @retail 0x1cae40
void function_1cae40(s_cluster_partition *partition, long data_index, long *first_cluster_reference)
{
	long reference_index = *first_cluster_reference;

	while (reference_index != NONE)
	{
		s_cluster_reference *reference = (s_cluster_reference *)cluster_partition_datum(partition->cluster_references, reference_index);
		short cluster_index = reference->cluster_index;
		long next_reference_index = reference->next_reference_index;
		long *link;
		s_record_pool *data_references;

		record_pool_release(partition->cluster_references, reference_index);
		link = &partition->cluster_first_data_references[cluster_index];
		data_references = partition->data_references;
		while (*link != NONE)
		{
			s_data_reference *data_reference = (s_data_reference *)cluster_partition_datum(data_references, *link);

			if (data_reference->data_index == data_index)
			{
				long next_data_reference_index = data_reference->next_reference_index;

				record_pool_release(data_references, *link);
				*link = next_data_reference_index;
				break;
			}
			link = &data_reference->next_reference_index;
		}
		reference_index = next_reference_index;
	}
	*first_cluster_reference = NONE;
}

struct s_partition_location
{
	byte unknown00[4];
	short cluster_index;
};

/* in src/unknown_1cac60.cpp, which retail builds with SSE */
void function_1cac60(dword const *bits, s_cluster_partition *partition, long data_index,
	long *first_cluster_reference, point3f const *point, real radius, s_partition_location const *location,
	long payload_size, void const *payload, bool *overflow);

void function_2962e0(s_record_pool *pool, long *head, long key, long size, void const *value);

// @retail 0x1cadf0
void function_1cadf0(s_cluster_partition *partition, long reference_index, long data_index, long payload_size, void const *payload)
{
	while (reference_index != NONE)
	{
		s_cluster_reference *reference = (s_cluster_reference *)cluster_partition_datum(partition->cluster_references, reference_index);
		function_2962e0(partition->data_references, &partition->cluster_first_data_references[reference->cluster_index], data_index, payload_size, payload);
		reference_index = reference->next_reference_index;
	}
}


void *function_123d40(char const *name, char const *type, long size);
char *function_11c9c0(char *buffer, long maximum_count, char const *format, ...);
s_record_pool *function_296270(char const *name, long size, long count);

PRIVATE __forceinline s_record_pool *function_1cabc1(long arg_0, long arg_1, char *arg_2, char const *arg_3, char const *arg_4)
{
 return function_296270(function_11c9c0(arg_2, 0x100, arg_3, arg_4), arg_0, arg_1);
}

// @retail 0x1cabc0
void function_1cabc0(s_cluster_partition *partition, char const *name, long payload_size)
{
	(void)&partition;
	(void)&name;
	(void)&payload_size;
	char label[256];
	partition->cluster_first_data_references = (long *)function_123d40(NULL, NULL, 0x800);
	partition->data_references = function_1cabc1(payload_size, 0x800, label, "cluster %s", name);
	partition->cluster_references = function_1cabc1(0, 0x800, label, "%s cluster", name);
}
