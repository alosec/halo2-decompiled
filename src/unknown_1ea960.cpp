#include "unknown_11c920.h"
#include "unknown_1efac0.h"
#include "unknown_1eb350.h"
#include "havok_reference.h"

// @flags /O2 /Gr

struct c_1ea960 : c_a
{
	virtual ~c_1ea960();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea960
c_1ea960::~c_1ea960()
{
#if H2_NATIVE_PORT
	__builtin_trap();
#else
	__asm int 3
	__assume(0);
#endif
}

struct c_1ea970 : c_a
{
	virtual ~c_1ea970();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea970
c_1ea970::~c_1ea970()
{
#if H2_NATIVE_PORT
	__builtin_trap();
#else
	__asm int 3
	__assume(0);
#endif
}

struct c_1ea980 : c_a
{
	virtual ~c_1ea980();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea980
c_1ea980::~c_1ea980()
{
#if H2_NATIVE_PORT
	__builtin_trap();
#else
	__asm int 3
	__assume(0);
#endif
}

struct c_1ea990 : c_a
{
	virtual ~c_1ea990();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea990
c_1ea990::~c_1ea990()
{
#if H2_NATIVE_PORT
	__builtin_trap();
#else
	__asm int 3
	__assume(0);
#endif
}

struct c_1ea9a0 : c_a
{
	virtual ~c_1ea9a0();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea9a0
c_1ea9a0::~c_1ea9a0()
{
#if H2_NATIVE_PORT
	__builtin_trap();
#else
	__asm int 3
	__assume(0);
#endif
}

struct c_1ea9b0 : c_a
{
	virtual ~c_1ea9b0();
	virtual void *v3() { return 0; }
};

// @retail 0x1ea9b0
c_1ea9b0::~c_1ea9b0()
{
#if H2_NATIVE_PORT
	__builtin_trap();
#else
	__asm int 3
	__assume(0);
#endif
}

struct c_shape_adapter_a : c_shape_library_base_a {};
struct c_shape_adapter_b : c_shape_library_base_b {};

// @retail 0x1eb3b0 deleting c_shape_adapter_a
// @retail 0x1eb3e0 deleting c_shape_adapter_b

struct c_shape_reference : c_a
{
	long field_8;
	c_havok_reference_counted *object;
	virtual ~c_shape_reference();
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
	}
};

// @retail 0x1eb320 deleting c_shape_reference

// @retail 0x1eb350
c_shape_reference::~c_shape_reference()
{
	havok_reference_remove(object);
}

struct c_shape_pair : c_a
{
	long field_8;
	c_havok_reference_counted *first;
	c_havok_reference_counted *second;
	virtual ~c_shape_pair();
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
	}
};

// @retail 0x1ea930 deleting c_shape_pair

// @retail 0x1eb4c0
c_shape_pair::~c_shape_pair()
{
	havok_reference_remove(second);
	havok_reference_remove(first);
}

struct s_shape_array16
{
	void *data;
	long count;
	long capacity;
	__forceinline ~s_shape_array16()
	{
		if (!(capacity & 0x80000000))
			g_480118->allocate((long)data, (capacity & 0x7fffffff) * 0x10, 0x12);
	}
};

struct s_shape_array48
{
	void *data;
	long count;
	long capacity;
	__forceinline ~s_shape_array48()
	{
		if (!(capacity & 0x80000000))
			g_480118->allocate((long)data, (capacity & 0x7fffffff) * 0x30, 0x12);
	}
};

struct c_shape_arrays : c_a
{
	byte field_8[0x30 - 8];
	s_shape_array48 first;
	byte field_3c[0xd4 - 0x3c];
	s_shape_array16 second;
	void operator delete(void *block)
	{
		g_480118->allocate((long)block, ((c_a *)block)->flags, 0x22);
	}
};

// @retail 0x1eb410 deleting c_shape_arrays

// @retail 0x1eb440 destructor c_shape_arrays

#include "unknown_1cec30.h"

long havok_entity_component_index_get(hkEntity const *entity);
long havok_entity_property_2002_get(hkEntity const *entity);
void function_1d2070(hkEntity const *entity, s_havok_component *component,
 long component_index, long kind, long shape_index);

struct s_contact_body
{
 byte field_0[0x18];
 long kind;
 long field_1c;
 hkEntity *entity;
};

struct c_shape_contact_listener
{
 long field_4;
 long kind;
 virtual void added(s_contact_body const *a, s_contact_body const *b, void *contact);
 virtual void removed(s_contact_body const *a, s_contact_body const *b);
};

// @retail 0x1eb2a0
void c_shape_contact_listener::removed(s_contact_body const *a, s_contact_body const *b)
{
 long contact_kind = kind;
 hkEntity *first = a->kind == 1 ? a->entity : NULL;
 hkEntity *second = b->kind == 1 ? b->entity : NULL;
 if (first && second)
 {
  long component_index = havok_entity_component_index_get(first);
  if (component_index != NONE)
  {
   long shape_index = havok_entity_property_2002_get(first);
   function_1d2070(second, havok_component_get(component_index), component_index,
    shape_index, contact_kind);
  }
 }
}

struct c_material_shape
{
 long field_4;
 byte *user;
 c_material_shape *child;
 virtual void v0() {}
 virtual void v1() {}
 virtual void v2() {}
 virtual void v3() {}
 virtual void v4() {}
 virtual long kind() { return 0; }
 virtual void v6() {}
 virtual void v7() {}
 virtual void v8() {}
 virtual void v9() {}
 virtual void v10() {}
 virtual long first_key(void *buffer) { return NONE; }
 virtual void v12() {}
 virtual c_material_shape *get_child(long key) { return NULL; }
};

struct s_globals_element;
s_globals_element *function_188640(long key);

// @retail 0x1eb020
long function_1eb020(long tag_index, long body_index, short *material_index)
{
 (void)&material_index;
#if H2_NATIVE_PORT
 __declspec(align(16)) byte buffer[0x100];
#else
 __m128 buffer[16];
#endif
 byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
 byte *body = *(byte **)(definition + 0x3c) + body_index * 0x90;
 c_material_shape *shape = *(c_material_shape **)(body + 0x38);
 long result = 0;
 if (shape->kind() == 0x13) shape = shape->child;
 long kind = shape->kind();
 if (kind == 2 || (kind > 9 && kind <= 11))
 {
  shape = shape->get_child(shape->first_key(buffer));
  if (shape->kind() == 0x11) shape = shape->child;
  if (shape->kind() == 0x15) shape = shape->child;
 }
 else
 {
  if (shape->kind() == 0x11) shape = shape->child;
  if (shape->kind() == 0x15) shape = shape->child;
 }
 byte *data = shape->user;
 short index = *(short *)(data + 4);
 if (index != NONE)
 {
  long *entry = (long *)(*(byte **)(definition + 0x44) + index * 12);
  result = entry[1];
  if (!function_188640(result)) result = entry[0];
 }
 *material_index = *(short *)(data + 6);
 return result;
}

void function_1d1e40(hkEntity const *entity, s_havok_component *component, long component_index, long kind, long shape_index);

// @retail 0x1eb220
void c_shape_contact_listener::added(s_contact_body const *a, s_contact_body const *b, void *contact)
{
 long contact_kind = kind;
 hkEntity *first = a->kind == 1 ? a->entity : NULL;
 hkEntity *second = b->kind == 1 ? b->entity : NULL;
 if (first && second)
 {
  long component_index = havok_entity_component_index_get(first);
  if (component_index != NONE)
  {
   long shape_index = havok_entity_property_2002_get(first);
   function_1d1e40(second, havok_component_get(component_index), component_index, shape_index, contact_kind);
  }
 }
}
