// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4805b8
{
    long unknown0;
};

object_4805b8 g_4805b8;

struct class_30b610
{
    object_4805b8 *function_30b610();
};

// @retail 0x30b610
object_4805b8 *class_30b610::function_30b610()
{
    return &g_4805b8;
}
