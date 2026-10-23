// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480630
{
    long unknown0;
};

object_480630 g_480630;

struct class_3122a0
{
    object_480630 *function_3122a0();
};

// @retail 0x3122a0
object_480630 *class_3122a0::function_3122a0()
{
    return &g_480630;
}
