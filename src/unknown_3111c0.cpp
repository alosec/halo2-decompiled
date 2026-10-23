// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4805e0
{
    long unknown0;
};

object_4805e0 g_4805e0;

struct class_3111c0
{
    object_4805e0 *function_3111c0();
};

// @retail 0x3111c0
object_4805e0 *class_3111c0::function_3111c0()
{
    return &g_4805e0;
}
