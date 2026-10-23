// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480348
{
    long unknown0;
};

object_480348 g_480348;

struct class_2df3f0
{
    object_480348 *function_2df3f0();
};

// @retail 0x2df3f0
object_480348 *class_2df3f0::function_2df3f0()
{
    return &g_480348;
}
