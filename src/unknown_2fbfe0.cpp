// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4803ec
{
    long unknown0;
};

object_4803ec g_4803ec;

struct class_2fbfe0
{
    object_4803ec *function_2fbfe0();
};

// @retail 0x2fbfe0
object_4803ec *class_2fbfe0::function_2fbfe0()
{
    return &g_4803ec;
}
