// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_48043c
{
    long unknown0;
};

object_48043c g_48043c;

struct class_2fc540
{
    object_48043c *function_2fc540();
};

// @retail 0x2fc540
object_48043c *class_2fc540::function_2fc540()
{
    return &g_48043c;
}
