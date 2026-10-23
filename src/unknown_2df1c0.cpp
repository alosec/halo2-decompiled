// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4802a8
{
    long unknown0;
};

object_4802a8 g_4802a8;

struct class_2df1c0
{
    object_4802a8 *function_2df1c0();
};

// @retail 0x2df1c0
object_4802a8 *class_2df1c0::function_2df1c0()
{
    return &g_4802a8;
}
