// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480608
{
    long unknown0;
};

object_480608 g_480608;

struct class_3112d0
{
    object_480608 *function_3112d0();
};

// @retail 0x3112d0
object_480608 *class_3112d0::function_3112d0()
{
    return &g_480608;
}
