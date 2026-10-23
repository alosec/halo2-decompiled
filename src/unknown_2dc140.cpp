// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_48027c
{
    long unknown0;
};

object_48027c g_48027c;

struct class_2dc140
{
    object_48027c *function_2dc140();
};

// @retail 0x2dc140
object_48027c *class_2dc140::function_2dc140()
{
    return &g_48027c;
}
