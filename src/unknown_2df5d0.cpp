// @flags /O2 /GL-
// Havok (third-party physics library): forwards to a virtual method of
// the object held at offset 0xc.

struct target_2df5d0
{
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual long forwarded() = 0;
};

struct class_2df5d0
{
    char unknown0[0xc];
    target_2df5d0 *field_c;

    long function_2df5d0();
};

// @retail 0x2df5d0
long class_2df5d0::function_2df5d0()
{
    return field_c->forwarded();
}
