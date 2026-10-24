// @flags /O2 /GL-
// Havok (third-party physics library): reports failure through its
// argument.

#if H2_WHOLE_IMAGE
/* an hkBool returned by value: a class, so through the hidden result
   pointer that retail's callers pass (x8 on the port's ARM guest, where an
   explicit pointer argument would arrive elsewhere) */
struct hkbool_2fe820
{
    char value;
    hkbool_2fe820() {}
};

struct class_2fe820
{
    hkbool_2fe820 function_2fe820();
};

// @retail 0x2fe820
hkbool_2fe820 class_2fe820::function_2fe820()
{
    hkbool_2fe820 result;
    result.value = false;
    return result;
}
#else
struct class_2fe820
{
    void function_2fe820(bool *result);
};

// @retail 0x2fe820
void class_2fe820::function_2fe820(bool *result)
{
    *result = false;
}
#endif
