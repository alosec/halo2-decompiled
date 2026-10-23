// @flags /O2 /GL-
// Havok (third-party physics library): forwards to a virtual method of
// the object held at offset 0x8.

struct target_2d9060
{
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual long forwarded() = 0;
};

struct class_2d9060
{
    char unknown0[0x8];
    target_2d9060 *field_8;

    long function_2d9060();
};

// @retail 0x2d9060
long class_2d9060::function_2d9060()
{
    return field_8->forwarded();
}
