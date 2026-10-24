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
#if H2_WHOLE_IMAGE
    /* slot 10 returns a 12-byte descriptor by value (through a hidden
       pointer) and slot 11 takes an output pointer, both passed on by
       retail's jmp */
    struct descriptor { long words[3]; };
    virtual descriptor forwarded_a() = 0;
    virtual void *forwarded_b(void *out) = 0;
#else
    virtual long forwarded_a() = 0;
    virtual long forwarded_b() = 0;
#endif
};

struct class_2dbef0
{
    char unknown0[0x30];
    target_2dbef0 *field_30;

#if H2_WHOLE_IMAGE
    target_2dbef0::descriptor function_2dbef0();
    void *function_2dbf00(void *out);
#else
    long function_2dbef0();
    long function_2dbf00();
#endif
};

#if H2_WHOLE_IMAGE
// @retail 0x2dbef0
target_2dbef0::descriptor class_2dbef0::function_2dbef0()
{
    return field_30->forwarded_a();
}

// @retail 0x2dbf00
void *class_2dbef0::function_2dbf00(void *out)
{
    return field_30->forwarded_b(out);
}
#else
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
#endif
