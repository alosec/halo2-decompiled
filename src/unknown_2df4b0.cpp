// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_480370
{
    long unknown0;
};

object_480370 g_480370;

struct class_2df4b0
{
    object_480370 *function_2df4b0();
};

// @retail 0x2df4b0
object_480370 *class_2df4b0::function_2df4b0()
{
    return &g_480370;
}
