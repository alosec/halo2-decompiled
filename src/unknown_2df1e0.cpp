// @flags /O2 /GL-
// Havok (third-party physics library): a constructor that clears its only
// field, the address of a static object, and a setter and getter of the
// field at offset 8.

struct class_2df1e0
{
    long field_0;

    class_2df1e0();
};

// @retail 0x2df1e0
class_2df1e0::class_2df1e0()
{
    field_0 = 0;
}

struct object_4802f8
{
    long unknown0;
};

object_4802f8 g_4802f8;

struct class_2df1f0
{
    char unknown0[8];
    long field_8;

    object_4802f8 *function_2df1f0();
    void function_2df200(long const *value);
    void function_2df210(long *value) const;
};

// @retail 0x2df1f0
object_4802f8 *class_2df1f0::function_2df1f0()
{
    return &g_4802f8;
}

// @retail 0x2df200
void class_2df1f0::function_2df200(long const *value)
{
    field_8 = *value;
}

// @retail 0x2df210
void class_2df1f0::function_2df210(long *value) const
{
    *value = field_8;
}
