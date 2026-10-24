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
#if H2_WHOLE_IMAGE
    /* slot 6 is get_bounds(transform, float tolerance, out): retail's jmp
       passes all three on */
    virtual void forwarded(void const *transform, float tolerance, void *out) = 0;
#else
    virtual long forwarded() = 0;
#endif
};

struct class_2df5d0
{
    char unknown0[0xc];
    target_2df5d0 *field_c;

#if H2_WHOLE_IMAGE
    void function_2df5d0(void const *transform, float tolerance, void *out);
#else
    long function_2df5d0();
#endif
};

#if H2_WHOLE_IMAGE
// @retail 0x2df5d0
void class_2df5d0::function_2df5d0(void const *transform, float tolerance, void *out)
{
    field_c->forwarded(transform, tolerance, out);
}
#else
// @retail 0x2df5d0
long class_2df5d0::function_2df5d0()
{
    return field_c->forwarded();
}
#endif
