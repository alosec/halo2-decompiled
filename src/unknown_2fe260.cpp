// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480554
{
    long unknown0;
};

object_480554 g_480554;

struct class_2fe260
{
    object_480554 *function_2fe260();
};

// @retail 0x2fe260
object_480554 *class_2fe260::function_2fe260()
{
    return &g_480554;
}
