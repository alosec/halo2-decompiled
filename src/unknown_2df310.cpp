// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480320
{
    long unknown0;
};

object_480320 g_480320;

struct class_2df310
{
    object_480320 *function_2df310();
};

// @retail 0x2df310
object_480320 *class_2df310::function_2df310()
{
    return &g_480320;
}
