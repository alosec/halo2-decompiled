#ifndef DERIVED_311920_H
#define DERIVED_311920_H
// Havok (third-party physics library): an object built on another (0x311890
// builds that), adding only its own table (0x311920).
#include "reference_counted_2d88a0.h"
#include "description_312430.h"

struct s_3114b0;
void __cdecl function_3114b0(s_3114b0 *object);

// a class's own operator new: a block from the allocator with the tag 0x29,
// its size recorded in the object; operator delete gives it back through
// 0x3114b0
#define OBJECT_NEW_3114B0 \
	static __forceinline void *operator new(unsigned int size) \
	{ \
		referenced_2d88a0 *object = (referenced_2d88a0 *)g_memory_2d88a0->allocate_block(size, 0x29); \
		object->memory_size = (unsigned short)size; \
		return object; \
	} \
	static void operator delete(void *object) \
	{ \
		function_3114b0((s_3114b0 *)object); \
	}

// a rotation matrix set from a quaternion (0x372500)
struct rotation_372500
{
	__m128 column[3];

	void function_372500(__m128 const *quaternion);
};

// the table and the plain fields come first, so that the table does not take
// 16 bytes as it would in a class holding vectors itself
class header_3114d0
{
public:
	header_3114d0() : field_8(0) {}
	virtual ~header_3114d0() {}

	unsigned short memory_size;
	unsigned short reference_count;
	long field_8;
	float field_c;
	long field_10;
	long field_14;
	long field_18;
	long field_1c;
};

// the object 0x3114d0 builds, which the first one is built on: a position
// (a) and a rotation (b) with the transform made from them
class class_3114d0 : public header_3114d0
{
public:
	class_3114d0(void const *a, void const *b);
	virtual ~class_3114d0() {}

	__m128 field_20;
	__m128 rotation;
	__m128 field_40;
	__m128 field_50;
	__m128 field_60;
	__m128 field_70;
	rotation_372500 transform_rotation;
	__m128 transform_translation;
};

// four words that start cleared
struct quad_311890
{
	long value[4];

	quad_311890()
	{
		value[0] = 0;
		value[1] = 0;
		value[2] = 0;
		value[3] = 0;
	}
};

class class_311890 : public class_3114d0
{
public:
	class_311890(void const *a, void const *b);
	virtual ~class_311890() {}

	OBJECT_NEW_3114B0

	quad_311890 field_c0;
	quad_311890 field_d0;
	quad_311890 field_e0;
	quad_311890 field_f0;
};

class class_311920 : public class_311890
{
public:
	class_311920(void const *a, void const *b);
	virtual ~class_311920() {}

	OBJECT_NEW_3114B0
};

#endif
