// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of a static
// object in .data.

struct object_4801ac
{
    long unknown0;
};

object_4801ac g_4801ac;

struct class_2da040
{
    object_4801ac *function_2da040();
};

// @retail 0x2da040
object_4801ac *class_2da040::function_2da040()
{
    return &g_4801ac;
}
