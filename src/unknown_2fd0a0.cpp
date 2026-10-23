// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_48048c
{
    long unknown0;
};

object_48048c g_48048c;

struct class_2fd0a0
{
    object_48048c *function_2fd0a0();
};

// @retail 0x2fd0a0
object_48048c *class_2fd0a0::function_2fd0a0()
{
    return &g_48048c;
}
