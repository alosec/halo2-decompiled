// @flags /O2 /GL-
// Havok (third-party physics library): returns the address of the member
// at offset 0x10; its argument is unused.

struct class_2ddcc0
{
    char unknown0[0x10];
    long field_10;

    long *function_2ddcc0(long arg_0);
};

// @retail 0x2ddcc0
long *class_2ddcc0::function_2ddcc0(long arg_0)
{
    return &field_10;
}
