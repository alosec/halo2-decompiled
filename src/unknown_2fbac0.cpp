// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_48039c
{
    long unknown0;
};

object_48039c g_48039c;

struct class_2fbac0
{
    object_48039c *function_2fbac0();
};

// @retail 0x2fbac0
object_48039c *class_2fbac0::function_2fbac0()
{
    return &g_48039c;
}
