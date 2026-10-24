// @flags /O2 /GL-
// Havok (third-party physics library): two class entries' functions, from
// the pairs of words at 0x415404 (next to the class names "MiNumJacobians"
// and "hkFakeRigidBodyDeactivator"):
//   0x31c4f0  makes the objects' collision filter (vtable 0x415100, 8 bytes,
//             tag 0x26; its slots are in havok_contact_mgr_318a80.cpp)
//   0x31c4b0  builds an object in place on the memory given (the base
//             0x31bc60 of the deactivator's settings), returning it
// and the pair at 0x43e544:
//   0x372bf0  makes an object of 0x18 bytes (tag 0x22, vtable 0x4138dc)
//   0x372bb0  as 0x31c4b0, through 0x31bc70
#include "reference_counted_2d88a0.h"

// the settings' base constructor (port/recovered/havok_deactivator_314710.cpp)
extern "C" void *h2_retail_31bc60(void *self);

// @retail 0x31c4f0
void *__cdecl function_31c4f0()
{
	referenced_2d88a0 *filter = (referenced_2d88a0 *)g_memory_2d88a0->allocate_block(8, 0x26);
	filter->memory_size = 8;
	filter->reference_count = 1;
	*(void const **)filter = (void const *)0x415100;
	return filter;
}

// retail returns whatever eax held when the memory is null; here null
// @retail 0x31c4b0
void *__cdecl function_31c4b0(void *memory)
{
	if (memory)
	{
		return h2_retail_31bc60(memory);
	}
	return memory;
}

// ---------------------------------------------------------------------------
// the pair of words at 0x43e544

// gives back the object it is passed (src/unknown_31bc70.cpp)
void *__fastcall function_31bc70(void *arg_0);

// as 0x31c4b0, with 0x31bc70 itself (retail returns whatever eax held when
// the memory is null; here null)
// @retail 0x372bb0
void *__cdecl function_372bb0(void *memory)
{
	if (memory)
	{
		return function_31bc70(memory);
	}
	return memory;
}

// an object of 0x18 bytes (tag 0x22, vtable 0x4138dc) holding four
// interfaces of its own (vtables 0x4138c8, 0x4138d4, 0x4138cc, 0x4138c8;
// retail first stores their bases' 0x412588, 0x455570, 0x455590, 0x412588)
struct object_372bf0
{
	void const *vtable;
	unsigned short memory_size;
	unsigned short reference_count;
	void const *interfaces[4];
};

typedef char assert_object_372bf0_size[sizeof(object_372bf0) == 0x18 ? 1 : -1];

// @retail 0x372bf0
void *__cdecl function_372bf0()
{
	object_372bf0 *object = (object_372bf0 *)g_memory_2d88a0->allocate_block(0x18, 0x22);
	object->memory_size = 0x18;
	object->reference_count = 1;
	object->vtable = (void const *)0x4138dc;
	object->interfaces[0] = (void const *)0x4138c8;
	object->interfaces[1] = (void const *)0x4138d4;
	object->interfaces[2] = (void const *)0x4138cc;
	object->interfaces[3] = (void const *)0x4138c8;
	return object;
}
