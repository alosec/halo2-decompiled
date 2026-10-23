// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4804b4
{
    long unknown0;
};

object_4804b4 g_4804b4;

struct class_2fde30
{
    object_4804b4 *function_2fde30();
};

// @retail 0x2fde30
object_4804b4 *class_2fde30::function_2fde30()
{
    return &g_4804b4;
}
