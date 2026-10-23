// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480504
{
    long unknown0;
};

object_480504 g_480504;

struct class_2fdfe0
{
    object_480504 *function_2fdfe0();
};

// @retail 0x2fdfe0
object_480504 *class_2fdfe0::function_2fdfe0()
{
    return &g_480504;
}
