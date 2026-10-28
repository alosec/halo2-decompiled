// @flags /O2 /GL-
// Havok (third-party physics library): reports failure through its
// argument.

#include "buffered_writer_2d9e40.h"

#if H2_WHOLE_IMAGE
/* an hkBool returned by value: a class, so through the hidden result
   pointer that retail's callers pass (x8 on the port's ARM guest, where an
   explicit pointer argument would arrive elsewhere). 0x2fe820 is shared by
   several tables (default collision answers among them), so the port binds
   this form to the address */
struct hkbool_2fe820
{
    char value;
    hkbool_2fe820() {}
};

struct class_2fe820
{
    hkbool_2fe820 function_2fe820();
};

// @retail 0x2fe820
hkbool_2fe820 class_2fe820::function_2fe820()
{
    hkbool_2fe820 result;
    result.value = false;
    return result;
}

/* the stream writer's own slot, as main declares it (not bound to the
   address here) */
void stream_writer_2d9e40::query(bool *result)
{
    *result = false;
}
#else
// @retail 0x2fe820
void stream_writer_2d9e40::query(bool *result)
{
    *result = false;
}
#endif
