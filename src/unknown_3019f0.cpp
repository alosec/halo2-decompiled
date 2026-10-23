// @flags /O2 /GL-
// Havok (third-party physics library): sets the flag at offset 4; its two
// arguments are unused.

struct class_3019f0
{
    long unknown0;
    bool field_4;

    void function_3019f0(long arg_0, long arg_1);
};

// @retail 0x3019f0
void class_3019f0::function_3019f0(long arg_0, long arg_1)
{
    field_4 = true;
}
