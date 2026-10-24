// @flags /O2 /GL-
// Havok (third-party physics library): three methods that report success
// through their first argument; the other arguments are unused.

#if H2_WHOLE_IMAGE
/* the result is an hkBool returned by value: a class, so through the hidden
   pointer retail's callers pass (x8 on the port's ARM guest, where an
   explicit first argument would arrive elsewhere) */
struct hkbool_30d200
{
    char value;
    hkbool_30d200() {}
};

struct class_30d200
{
    hkbool_30d200 function_30d200(long arg_1, long arg_2, long arg_3, long arg_4, long arg_5);
    hkbool_30d200 function_30d210(long arg_1, long arg_2, long arg_3);
    hkbool_30d200 function_30d220(long arg_1, long arg_2);
};

// @retail 0x30d200
hkbool_30d200 class_30d200::function_30d200(long arg_1, long arg_2, long arg_3, long arg_4, long arg_5)
{
    hkbool_30d200 result;
    result.value = true;
    return result;
}

// @retail 0x30d210
hkbool_30d200 class_30d200::function_30d210(long arg_1, long arg_2, long arg_3)
{
    hkbool_30d200 result;
    result.value = true;
    return result;
}

// @retail 0x30d220
hkbool_30d200 class_30d200::function_30d220(long arg_1, long arg_2)
{
    hkbool_30d200 result;
    result.value = true;
    return result;
}
#else
struct class_30d200
{
    void function_30d200(bool *result, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5);
    void function_30d210(bool *result, long arg_1, long arg_2, long arg_3);
    void function_30d220(bool *result, long arg_1, long arg_2);
};

// @retail 0x30d200
void class_30d200::function_30d200(bool *result, long arg_1, long arg_2, long arg_3, long arg_4, long arg_5)
{
    *result = true;
}

// @retail 0x30d210
void class_30d200::function_30d210(bool *result, long arg_1, long arg_2, long arg_3)
{
    *result = true;
}

// @retail 0x30d220
void class_30d200::function_30d220(bool *result, long arg_1, long arg_2)
{
    *result = true;
}
#endif
