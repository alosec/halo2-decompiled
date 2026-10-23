// @flags /O2 /GL-
// Havok (third-party physics library): a field accessor.

struct class_2fc1a0
{
    char unknown0[0x10];
    long field_10;

    long function_2fc1a0() const;
};

// @retail 0x2fc1a0
long class_2fc1a0::function_2fc1a0() const
{
    return field_10;
}
