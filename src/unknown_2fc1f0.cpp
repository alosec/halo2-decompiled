// @flags /O2 /GL-
// Havok (third-party physics library): reads the first word of an 8-byte
// entry of the array at offset 0xc.

struct entry_2fc1f0
{
    long value;
    long unknown4;
};

struct class_2fc1f0
{
    char unknown0[0xc];
    entry_2fc1f0 *entries;

    long function_2fc1f0(long index) const;
};

// @retail 0x2fc1f0
long class_2fc1f0::function_2fc1f0(long index) const
{
    return entries[index].value;
}
