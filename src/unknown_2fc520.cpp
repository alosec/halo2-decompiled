// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480414
{
    long unknown0;
};

object_480414 g_480414;

struct class_2fc520
{
    object_480414 *function_2fc520();
};

// @retail 0x2fc520
object_480414 *class_2fc520::function_2fc520()
{
    return &g_480414;
}
