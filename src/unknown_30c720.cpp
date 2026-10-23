// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of the member
// at offset 8; its three arguments are unused.

struct class_30c720
{
    char unknown0[8];
    long field_8;

    long *function_30c720(long arg_0, long arg_1, long arg_2);
};

// @retail 0x30c720
long *class_30c720::function_30c720(long arg_0, long arg_1, long arg_2)
{
    return &field_8;
}
