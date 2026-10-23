// @flags /O2 /GL-
// Havok (third-party physics library): two methods that forward to virtual
// methods of the object held at offset 0x30.

struct target_2dbef0
{
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual long forwarded_a() = 0;
    virtual long forwarded_b() = 0;
};

struct class_2dbef0
{
    char unknown0[0x30];
    target_2dbef0 *field_30;

    long function_2dbef0();
    long function_2dbf00();
};

// @retail 0x2dbef0
long class_2dbef0::function_2dbef0()
{
    return field_30->forwarded_a();
}

// @retail 0x2dbf00
long class_2dbef0::function_2dbf00()
{
    return field_30->forwarded_b();
}
