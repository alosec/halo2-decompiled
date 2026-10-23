// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4804dc
{
    long unknown0;
};

object_4804dc g_4804dc;

struct class_2fdfa0
{
    object_4804dc *function_2fdfa0();
};

// @retail 0x2fdfa0
object_4804dc *class_2fdfa0::function_2fdfa0()
{
    return &g_4804dc;
}
