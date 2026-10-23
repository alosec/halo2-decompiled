// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_48052c
{
    long unknown0;
};

object_48052c g_48052c;

struct class_2fe0b0
{
    object_48052c *function_2fe0b0();
};

// @retail 0x2fe0b0
object_48052c *class_2fe0b0::function_2fe0b0()
{
    return &g_48052c;
}
