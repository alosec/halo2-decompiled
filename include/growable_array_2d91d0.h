#ifndef GROWABLE_ARRAY_2D91D0_H
#define GROWABLE_ARRAY_2D91D0_H
// Havok (third-party physics library): its growable arrays (data, size, and a
// capacity whose top bit says the array does not own its storage), and the
// helpers that reallocate them.
#include "reference_counted_2d88a0.h"

struct array_2d91d0
{
	void *data;
	long size;
	long capacity_and_flags;
};

// copies `count` bytes
void __cdecl function_2d8db0(void *destination, void const *source, long count);
// reallocates the array with room for `capacity` elements
void __cdecl function_2d9160(void *array, long capacity, long element_size);
// doubles the array's capacity (to 1 when it was 0)
void __cdecl function_2d91d0(void *array, long element_size);

// the members of the same array with elements of type `type`, and its inline
// operations (macros, not a template: the build instantiates templates where
// inlining is off)
#define ELEMENTS_2D91D0_MEMBERS(type) \
	type *data; \
	long size; \
	long capacity_and_flags; \
	/* the slot after the last element, the array grown first when full */ \
	__forceinline type &next_slot() \
	{ \
		if (size == (capacity_and_flags & 0x7fffffff)) \
		{ \
			function_2d91d0(this, sizeof(type)); \
		} \
		return data[size++]; \
	} \
	/* stores a copy of `value` after the last element, the array grown \
	   first when full */ \
	__forceinline void append_copy(type const &value) \
	{ \
		if (size == (capacity_and_flags & 0x7fffffff)) \
		{ \
			function_2d91d0(this, sizeof(type)); \
		} \
		data[size] = value; \
		size++; \
	} \
	/* room for at least `count` elements, at least twice the old room */ \
	__forceinline void ensure_capacity(long count) \
	{ \
		long capacity = capacity_and_flags & 0x7fffffff; \
		if (capacity < count) \
		{ \
			capacity *= 2; \
			function_2d9160(this, count >= capacity ? count : capacity, sizeof(type)); \
		} \
	} \
	__forceinline void resize(long count) \
	{ \
		ensure_capacity(count); \
		size = count; \
	} \
	__forceinline type &operator[](long index) { return data[index]; } \
	__forceinline type const &operator[](long index) const { return data[index]; } \
	__forceinline void remove_at(long index) \
	{ \
		size--; \
		(*this)[index] = (*this)[size]; \
	}

// declares `name`, the array of `type`, which can also append an element
// passed by value
#define ELEMENTS_2D91D0(name, type) \
	struct name \
	{ \
		ELEMENTS_2D91D0_MEMBERS(type) \
		/* stores `value` after the last element, the array grown first when full */ \
		__forceinline void append(type value) \
		{ \
			if (size == (capacity_and_flags & 0x7fffffff)) \
			{ \
				function_2d91d0(this, sizeof(type)); \
			} \
			data[size] = value; \
			size++; \
		} \
	}

// declares `name`, the array of `type`, for an element type with 16-byte
// alignment, which cannot be passed by value
#define ELEMENTS_2D91D0_ALIGNED(name, type) \
	struct name \
	{ \
		ELEMENTS_2D91D0_MEMBERS(type) \
	}

#endif
