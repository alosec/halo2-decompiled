// @flags /O2 /Ob1 /GL-
// Havok (third-party physics library): the destructor of a class with a
// vtable (0x41387c) and nothing to release, which gives its 4 bytes back to
// the allocator with tag 0x1c.
#include "reference_counted_2d88a0.h"

class class_30c640
{
public:
    virtual ~class_30c640();

    static void operator delete(void *object, unsigned int size)
    {
        g_memory_2d88a0->release_block(object, size, 0x1c);
    }
};

// @retail 0x30c610 deleting class_30c640

// @retail 0x30c640
class_30c640::~class_30c640()
{
}
