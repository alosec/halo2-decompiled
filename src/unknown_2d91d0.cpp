#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>

// @flags /O2 /Gr

struct s_havok_dynamic_array
{
    void *data;
    long size;
    dword capacity_and_flags;
};
static_assert(sizeof(s_havok_dynamic_array) == 0x0c, "s_havok_dynamic_array");

class c_havok_array_allocator
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void *allocate(long size, long category) = 0;
    virtual void release(void *block, long size, long category) = 0;
};

void function_2d8db0(void *destination, void const *source, long size);

// @retail 0x2d9160
void __cdecl function_2d9160(void *array_pointer, long new_capacity, long element_size)
{
    s_havok_dynamic_array *array = (s_havok_dynamic_array *)array_pointer;
    c_havok_array_allocator *allocator = (c_havok_array_allocator *)g_480118;
    void *new_data = allocator->allocate(new_capacity * element_size, 0x12);
#if H2_NATIVE_PORT
    if (array->size)
        memcpy(new_data, array->data, array->size * element_size);
#else
    function_2d8db0(new_data, array->data, array->size * element_size);
#endif
    if (!(array->capacity_and_flags & 0x80000000))
    {
        allocator->release(array->data,
            (array->capacity_and_flags & 0x7fffffff) * element_size, 0x12);
    }
    array->data = new_data;
    array->capacity_and_flags = new_capacity;
}

// @retail 0x2d91d0
void __cdecl function_2d91d0(void *array_pointer, long element_size)
{
    s_havok_dynamic_array *array = (s_havok_dynamic_array *)array_pointer;
    long new_capacity = array->size ? array->size * 2 : 1;
    c_havok_array_allocator *allocator = (c_havok_array_allocator *)g_480118;
    void *new_data = allocator->allocate(new_capacity * element_size, 0x12);
#if H2_NATIVE_PORT
    if (array->size)
        memcpy(new_data, array->data, array->size * element_size);
#else
    function_2d8db0(new_data, array->data, array->size * element_size);
#endif
    if (!(array->capacity_and_flags & 0x80000000))
    {
        allocator->release(array->data,
            (array->capacity_and_flags & 0x7fffffff) * element_size, 0x12);
    }
    array->data = new_data;
    array->capacity_and_flags = new_capacity;
}
