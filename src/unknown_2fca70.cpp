// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480464
{
    long unknown0;
};

object_480464 g_480464;

struct class_2fca70
{
    object_480464 *function_2fca70();
};

// @retail 0x2fca70
object_480464 *class_2fca70::function_2fca70()
{
    return &g_480464;
}
