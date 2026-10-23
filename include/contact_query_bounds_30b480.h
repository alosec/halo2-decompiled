#ifndef CONTACT_QUERY_BOUNDS_30B480_H
#define CONTACT_QUERY_BOUNDS_30B480_H
// Havok (third-party physics library): the axis-aligned box volume that
// contact queries add to the world, and what it is built from.
#include "reference_counted_2d88a0.h"
#include <xmmintrin.h>


struct c_contact_query_bounds_info
{
	long filter;
	char pad_4[0x1c];
	__m128 lower;
	__m128 upper;
};

struct array_30b480
{
	void *data;
	long size;
	unsigned long capacity_and_flags;

	array_30b480() : data(0), size(0), capacity_and_flags(0x80000000) {}
};

struct pair_30b480
{
	long field_0;
	long field_4;

	__forceinline pair_30b480() : field_0(0), field_4(0) {}
};

struct index_30b480
{
	long index;
	long field_4;
	long field_8;

	__forceinline index_30b480() : field_4(0), field_8(0)
	{
		index = -1;
	}
};

struct header_30b480
{
	pair_30b480 field_8;
	index_30b480 field_10;
};

struct filter_info_30b480
{
	long field_1c;
	long field_20;
	long type;
	long filter;

	__forceinline filter_info_30b480() : field_1c(0), field_20(0), type(2), filter(0) {}
};

struct collidable_30b480
{
	header_30b480 header;
	filter_info_30b480 info;
	void *owner;
	array_30b480 first;
	array_30b480 second;
	array_30b480 third;

	__forceinline collidable_30b480() : owner(0) {}
};

class volume_base_30b480 : public referenced_2d88a0
{
public:
	collidable_30b480 collidable;

	__forceinline volume_base_30b480()
	{
		collidable.owner = this;
	}
};

class c_contact_query_bounds_volume : public volume_base_30b480
{
public:
	__m128 lower;
	__m128 upper;
	array_30b480 bodies;

	c_contact_query_bounds_volume(c_contact_query_bounds_info const *info);

	REFERENCED_NEW_2D88A0(0x2c)
};

#endif
