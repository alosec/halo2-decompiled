// @flags /O2 /Gr /arch:SSE
/* UNKNOWN_1CAC60.CPP: adds a thing to the cluster partition (see
   src/unknown_1cabc0.cpp): it links the thing into every cluster it touches.
   Its own file because retail builds it with SSE and its neighbours without */

#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include <string.h>

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

struct s_partition_location
{
	byte unknown00[4];
	short cluster_index;
};

#include "cluster_marks_storage.h"
short structure_clusters_from_bit_vector(dword const *bits, short *count, short maximum_count, short *clusters);
short __stdcall function_14a5b0(short cluster_index, point3f const *point, real radius, short maximum_count, short *clusters);

// @retail 0x1cac60
void function_1cac60(dword const *bits, s_cluster_partition *partition, long data_index,
	long *first_cluster_reference, point3f const *point, real radius, s_partition_location const *location,
	long payload_size, void const *payload, bool *overflow)
{
	(void)&partition;
	(void)&data_index;
	(void)&first_cluster_reference;
	(void)&point;
	(void)&radius;
	(void)&location;
	(void)&payload_size;
	(void)&payload;
	(void)&overflow;
	short clusters[0x100];
	short total;
	short count;
	if (bits)
		count = structure_clusters_from_bit_vector(bits, &total, 0x100, clusters);
	else
	{
		count = 0;
		short cluster = location->cluster_index;
		if (cluster != NONE)
		{
			if (radius > 0.0f)
			{
				++g_4e7414;
				g_4e7411 = true;
				count = function_14a5b0(cluster, point, radius, 0x100, clusters);
				g_4e7411 = false;
			}
			else
			{
				count = 1;
				clusters[0] = cluster;
			}
		}
		total = count;
		if (count > 0x100)
			count = 0x100;
	}
	*overflow = total > count;
	if (count > 0)
	{
	short *current = clusters;
	long remaining = (word)count;
	do
	{
		short cluster = *current;
		s_record_pool *cluster_references = partition->cluster_references;
		long reference_index = record_pool_allocate(cluster_references);
		if (reference_index != NONE)
		{
			s_data_reference *reference = (s_data_reference *)cluster_partition_datum(cluster_references, reference_index);
			reference->data_index = cluster;
			reference->next_reference_index = *first_cluster_reference;
			*first_cluster_reference = reference_index;
		}
		long *link = &partition->cluster_first_data_references[cluster];
		s_record_pool *data_references = partition->data_references;
		long index = record_pool_allocate(data_references);
		if (index != NONE)
		{
			s_data_reference *reference = (s_data_reference *)cluster_partition_datum(data_references, index);
			reference->data_index = data_index;
			reference->next_reference_index = *link;
			*link = index;
			if (payload)
				memcpy(reference + 1, payload, payload_size);
		}
		++current;
	} while (--remaining);
	}
}

