// @flags /O2 /GL-
// Havok (third-party physics library): forwards to a virtual method of
// the object held at offset 0x30.

struct target_2dbd30
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
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void v13() = 0;
#if H2_WHOLE_IMAGE
    /* retail's jmp passes the caller's argument (an output pointer) on */
    virtual long forwarded(void *argument) = 0;
#else
    virtual long forwarded() = 0;
#endif
};

struct class_2dbd30
{
    char unknown0[0x30];
    target_2dbd30 *field_30;

#if H2_WHOLE_IMAGE
    long function_2dbd30(void *argument);
#else
    long function_2dbd30();
#endif
};

#if H2_WHOLE_IMAGE
// @retail 0x2dbd30
long class_2dbd30::function_2dbd30(void *argument)
{
    return field_30->forwarded(argument);
}
#else
// @retail 0x2dbd30
long class_2dbd30::function_2dbd30()
{
    return field_30->forwarded();
}
#endif
