// @flags /O2 /GL-
// Havok (third-party physics library): reads the first word of an 8-byte
// entry of the array at offset 0xc; the second argument is unused.

struct entry_2fc1d0
{
    long value;
    long unknown4;
};

struct class_2fc1d0
{
    char unknown0[0xc];
    entry_2fc1d0 *entries;

    long function_2fc1d0(long index, long arg_1) const;
};

// @retail 0x2fc1d0
long class_2fc1d0::function_2fc1d0(long index, long arg_1) const
{
    return entries[index].value;
}
